# Dynamic Layered Nonlinear Cipher (DLNC)

DLNC is a completely new encryption algorithm designed for **security and efficiency**. It applies **key-dependent bit permutations, nonlinear modular mixing, and XOR+rotation layers** to encrypt 128-bit blocks. 

## Features
- Key-dependent, dynamic transformations
- Layered encryption (Permutation → Nonlinear Mixing → XOR+Rotation)
- Efficient CPU-friendly operations
- Fully reversible with proper modular inverses

## Requirements
- C++11 or later compiler (e.g., g++, clang++)
- Standard C++ libraries:
  - `<iostream>`, `<vector>`, `<array>`, `<algorithm>`, `<random>`
- Optional: CMake for cross-platform builds

## Usage

1. Include `dlcn.cpp` in your project.
2. Define a master key (8×16-bit segments).
3. Expand keys using `expandKey()`.
4. Encrypt plaintext blocks with `encryptBlock()`.
5. Decrypt ciphertext with `decryptBlock()` (ensure modular inverses implemented).

### Example

```cpp
#include "dlcn.cpp"

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