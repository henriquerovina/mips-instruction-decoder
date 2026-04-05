# Trace‑Driven MIPS Instruction Decoder 🖥️

A C program that parses a dynamic MIPS instruction trace using **only bitwise operations** to decode instructions and report execution statistics.

### 📍 Purpose  
Built for a computer architecture course to model the decode stage of a MIPS processor. Reads a trace of already‑executed `<PC, instruction>` pairs, reconstructs control flow by comparing consecutive PCs, and generates a formatted statistics file.

### ✨ Key Features  
* **Bitwise‑only decoding** – extracts opcode, funct, rs, rt, rd, immediates, and jump targets using shifts and masks (no string parsing).  
* **Dynamic branch analysis** – classifies each branch/jump as forward taken, backward taken, or not taken.  
* **Full statistics** – counts R/I/J‑type instructions; percentages of loads, stores, arithmetic; and read/write counts for all 32 registers.  
* **Exact output format** – produces `statistics.txt` matching the project specification.

### 🛠️ Tech Stack  
* **Language:** C (C99)  
* **Libraries:** `stdio.h`, `stdint.h`  
* **Operations:** Bitwise shifts, AND, OR, masks

### 🚀 How to Run   
1. Compile:  
   ```bash
   gcc -o decoder decoder.c
2. **Execute:**
   ```bash
   ./decoder
3. **View Results:**
   ```bash
   cat statistics.txt

### 📝 Manual Decoding Proof
See `manual_proof.txt` in the repository for a step‑by‑step bitwise extraction of opcode, rs, rt, rd, and immediate fields from three sample instructions.
