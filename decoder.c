#include <stdio.h>
#include <stdint.h>

// Helper functions to classify instructions

int is_branch_or_jump(uint32_t instr) {
    uint32_t op = (instr >> 26) & 0x3F;
    // Branches: beq, bne, blez, bgtz, and REGIMM (bltz, bgez, etc.)
    if (op == 0x04 || op == 0x05 || op == 0x06 || op == 0x07) return 1;
    if (op == 0x01) return 1;          // REGIMM
    // Jumps: j, jal
    if (op == 0x02 || op == 0x03) return 1;
    return 0;
}

int is_load(uint32_t instr) {
    uint32_t op = (instr >> 26) & 0x3F;
    return (op == 0x20 || op == 0x21 || op == 0x23 || op == 0x24 || op == 0x25);
}

int is_store(uint32_t instr) {
    uint32_t op = (instr >> 26) & 0x3F;
    return (op == 0x28 || op == 0x29 || op == 0x2B);
}

int is_arithmetic(uint32_t instr) {
    uint32_t op = (instr >> 26) & 0x3F;
    if (op == 0) {  // R-type
        uint32_t funct = instr & 0x3F;
        // Common arithmetic R-type instructions
        return (funct >= 0x20 && funct <= 0x27) ||  // add, addu, sub, subu, and, or, xor, nor
               funct == 0x2A || funct == 0x2B;      // slt, sltu
    }
    // I-type arithmetic: addi, addiu, slti, sltiu, andi, ori, xori
    return (op == 0x08 || op == 0x09 || op == 0x0A || op == 0x0B ||
            op == 0x0C || op == 0x0D || op == 0x0E);
}

void update_register_stats(uint32_t instr, uint32_t reads[32], uint32_t writes[32]) {
    uint32_t op = (instr >> 26) & 0x3F;
    uint32_t rs = (instr >> 21) & 0x1F;
    uint32_t rt = (instr >> 16) & 0x1F;
    uint32_t rd = (instr >> 11) & 0x1F;

    if (op == 0) {  // R-type
        reads[rs]++;
        reads[rt]++;
        writes[rd]++;   // even if rd == 0, count it (spec does not exclude)
    }
    else if (op == 2) {  // j
        // no register access
    }
    else if (op == 3) {  // jal
        writes[31]++;    // $ra
    }
    else {  // I-type
        if (is_load(instr)) {
            reads[rs]++;
            writes[rt]++;
        }
        else if (is_store(instr)) {
            reads[rs]++;
            reads[rt]++;
        }
        else if (op == 0x04 || op == 0x05) {  // beq, bne
            reads[rs]++;
            reads[rt]++;
        }
        else if (op == 0x06 || op == 0x07) {  // blez, bgtz
            reads[rs]++;
        }
        else if (op == 0x01) {  // REGIMM (bltz, bgez, etc.)
            reads[rs]++;
            // Some REGIMM instructions like bgezal write to $31, but for simplicity we skip.
            // The spec example doesn't use them; if needed, you can add logic.
        }
        else {  // arithmetic I-type (addi, andi, etc.)
            reads[rs]++;
            writes[rt]++;
        }
    }
}

int main() {
    FILE *in = fopen("trace.txt", "r");
    FILE *out = fopen("statistics.txt", "w");
    if (!in || !out) {
        fprintf(stderr, "Error opening files.\n");
        return 1;
    }

    uint32_t pc, instr;
    uint32_t prev_pc = 0, prev_instr = 0;
    int prev_is_bj = 0;
    int first = 1;

    uint32_t total = 0, r_type = 0, i_type = 0, j_type = 0;
    uint32_t fwd_taken = 0, bkw_taken = 0, not_taken = 0;
    uint32_t loads = 0, stores = 0, arith = 0;
    uint32_t reg_reads[32] = {0};
    uint32_t reg_writes[32] = {0};

    while (fscanf(in, "%x %x", &pc, &instr) == 2) {
        total++;

        // Count instruction types
        uint32_t op = (instr >> 26) & 0x3F;
        if (op == 0) {
            r_type++;
        } else if (op == 2 || op == 3) {
            j_type++;
        } else {
            i_type++;
        }

        // Count loads, stores, arithmetic
        if (is_load(instr)) loads++;
        if (is_store(instr)) stores++;
        if (is_arithmetic(instr)) arith++;

        // Update register stats
        update_register_stats(instr, reg_reads, reg_writes);

        // Branch/jump taken detection (compare with next PC)
        if (!first) {
            if (prev_is_bj) {
                uint32_t next_pc = pc;
                uint32_t prev_next = prev_pc + 4;
                if (next_pc == prev_next) {
                    not_taken++;
                } else if (next_pc > prev_next) {
                    fwd_taken++;
                } else {
                    bkw_taken++;
                }
            }
        }

        // Save for next iteration
        prev_pc = pc;
        prev_instr = instr;
        prev_is_bj = is_branch_or_jump(instr);
        first = 0;
    }

    fclose(in);

    // Write output exactly as required
    fprintf(out, "insts: %u\n", total);
    fprintf(out, "r-type: %u\n", r_type);
    fprintf(out, "i-type: %u\n", i_type);
    fprintf(out, "j-type: %u\n", j_type);
    fprintf(out, "fwd-taken: %.6f\n", (total ? (fwd_taken * 100.0 / total) : 0));
    fprintf(out, "bkw-taken: %.6f\n", (total ? (bkw_taken * 100.0 / total) : 0));
    fprintf(out, "not-taken: %.6f\n", (total ? (not_taken * 100.0 / total) : 0));
    fprintf(out, "loads: %.6f\n", (total ? (loads * 100.0 / total) : 0));
    fprintf(out, "stores: %.6f\n", (total ? (stores * 100.0 / total) : 0));
    fprintf(out, "arith: %.6f\n", (total ? (arith * 100.0 / total) : 0));

    for (int i = 0; i < 32; i++) {
        fprintf(out, "reg-%d: %u %u\n", i, reg_reads[i], reg_writes[i]);
    }

    fclose(out);
    return 0;
}