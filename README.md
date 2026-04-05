# Trace‑Driven MIPS Instruction Decoder 🖥️

A C program that parses a dynamic instruction trace of MIPS 32‑bit instructions, decodes each instruction using **only bitwise operations**, and reports detailed execution statistics (branch outcomes, instruction mix, and per‑register read/write counts).

### 📍 Purpose  
Built for a computer architecture course to model the **decode stage** of a MIPS processor. The program reads a trace of already‑executed instructions (PC + 32‑bit hex), classifies each instruction, reconstructs control flow by comparing consecutive PCs, and generates a formatted statistics file.

### ✨ Key Features  
* **Bitwise‑only decoding** – extracts opcode, funct, rs, rt, rd, immediates, and jump targets using shifts and masks (no string parsing).  
* **Dynamic control‑flow analysis** – classifies each branch/jump as:  
  - Forward taken  
  - Backward taken  
  - Not taken  
* **Comprehensive statistics** – counts R‑type, I‑type, J‑type; percentages of loads, stores, arithmetic; and **read/write counts for all 32 registers** (even unused ones).  
* **Robust input handling** – reads `<PC, instruction>` pairs in hex directly from `trace.txt`.  
* **Exact output format** – produces `statistics.txt` matching the project specification (no extra spaces, 6‑decimal percentages).

### 🛠️ Tech Stack  
* **Language:** C (C99)  
* **Libraries:** Standard I/O (`stdio.h`), `stdint.h`  
* **Key operations:** Bitwise shift (`>>`, `<<`), AND (`&`), OR (`|`), masks

### 🚀 How to Run  
1. Place your input trace file as `trace.txt` in the same directory as the executable.  
2. **Compile:**  
   ```bash
   gcc -o decoder decoder.c
3. **Execute:**
   ```bash
   ./decoder
4. **View Results:**
   ```bash
   cat statistics.txt

###📝 Manual Decoding Proof
* **See manual_proof.txt in the repository for a step‑by‑step bitwise extraction of opcode, rs, rt, rd, and immediate fields from three sample instructions.
