# Dynamic Layered Nonlinear Cipher (DLNC)

DLNC is a **novel encryption algorithm** designed for **security and efficiency**. It encrypts 128-bit blocks using **dynamic, key-dependent transformations**, including **bit permutations, nonlinear modular mixing, and XOR+rotation layers**, making it highly resistant to precomputation attacks and cryptanalysis.

---

## Algorithm Overview

DLNC divides a 128-bit block into 8×16-bit segments and applies multiple rounds of **three-layered transformations**:

### 1. Dynamic Bit Permutation
- Each round generates a **unique permutation table** from the round key and block index.
- Bits in the block are scrambled according to this table.
- **Purpose:** Introduces confusion; a small change in the key drastically changes the permutation.

### 2. Nonlinear Modular Mixing
- Segments are mixed with modular arithmetic:
  segment_j = (segment_j * (segment_(j+1) + roundKey[j])) mod Q

where `Q` is a prime slightly larger than 2^16 (e.g., 65537).
- Mixing propagates changes across all segments.
- **Purpose:** Provides diffusion, ensuring small plaintext changes affect multiple ciphertext bits.

### 3. XOR and Rotational Diffusion
- Each 16-bit segment is rotated by a number of bits determined by the round key.
- XOR is applied with the round key to increase nonlinearity.
- **Purpose:** Strengthens encryption via combined linear and nonlinear transformations.

### Key Expansion
- The master key (8×16-bit segments) is expanded into **round keys** for each round:
  R_i[j] = (K[j] XOR f(i)) + i

- Round keys are unique per round and dependent on the master key.
- **Purpose:** Prevents weak keys and ensures every round behaves differently.

---

## Encryption Process
1. Split plaintext into 128-bit blocks (8×16-bit segments).  
2. For each round:
 - Apply **bit permutation**  
 - Apply **nonlinear mixing**  
 - Apply **XOR + rotation**  
3. Concatenate all segments to produce ciphertext.

## Decryption Process
- Reverse the steps in **inverse order**:
1. XOR + rotation (inverse rotation)  
2. Inverse nonlinear mixing (requires modular inverses)  
3. Inverse permutation (reconstruct permutation table)  

> **Note:** Decryption requires correct modular inverse calculations for nonlinear mixing.

---

## Features
- **Dynamic, key-dependent transformations** resist precomputation attacks.  
- **Hybrid layers** combine permutation, modular arithmetic, and XOR for strong security.  
- **CPU-efficient operations:** rotation, XOR, modular arithmetic, lightweight shuffling.  
- **Customizable:** block size, round count, or primes can be adjusted for security/performance balance.

---

## Requirements
- C++11 or later compiler (`g++`, `clang++`, MSVC)  
- Standard C++ libraries: `<iostream>`, `<vector>`, `<array>`, `<algorithm>`, `<random>`  
- Optional: CMake for cross-platform builds

---

## Usage Example

```cpp
#include "dlcn.cpp" // Include the DLNC implementation

int main() {
  array<uint16_t,8> masterKey = {0x1234,0x5678,0x9ABC,0xDEF0,0x1111,0x2222,0x3333,0x4444};
  auto roundKeys = expandKey(masterKey);
  
  array<uint16_t,8> plaintext = {0xAAAA,0xBBBB,0xCCCC,0xDDDD,0x1111,0x2222,0x3333,0x4444};
  auto ciphertext = encryptBlock(plaintext, roundKeys, 0);

  cout << "Ciphertext: ";
  for(auto x : ciphertext) cout << hex << x << " ";
  cout << endl;

  auto decrypted = decryptBlock(ciphertext, roundKeys, 0);
  cout << "Decrypted: ";
  for(auto x : decrypted) cout << hex << x << " ";
  cout << endl;

  return 0;
}
  
