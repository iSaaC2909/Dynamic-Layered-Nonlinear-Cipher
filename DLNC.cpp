#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <algorithm>
#include <random>
#include <functional>

using namespace std;

// ------------------- Helper Functions ------------------- //

// Simple left rotation for 16-bit segments
uint16_t rotateLeft16(uint16_t val, unsigned int shift) {
    return (val << shift) | (val >> (16 - shift));
}

// Generate a key-dependent permutation table for 128 bits
array<int, 128> generatePermutation(uint64_t roundKey, int blockIndex) {
    array<int, 128> perm;
    for(int i=0; i<128; i++) perm[i] = i;
    seed_seq seq{static_cast<int>(roundKey), blockIndex};
    shuffle(perm.begin(), perm.end(), default_random_engine(seq));
    return perm;
}

// Nonlinear modular mixing for 16-bit segments
void nonlinearMix(array<uint16_t, 8>& segments, array<uint16_t,8>& roundKey) {
    const uint16_t Q = 65537; // prime slightly above 2^16
    for(int i=0; i<8; i++){
        segments[i] = (segments[i] * (segments[(i+1)%8] + roundKey[i])) % Q;
    }
}

// Key expansion (simple example)
array<array<uint16_t, 8>, 10> expandKey(const array<uint16_t, 8>& masterKey) {
    array<array<uint16_t, 8>, 10> roundKeys;
    for(int r=0; r<10; r++){
        for(int i=0; i<8; i++){
            roundKeys[r][i] = (masterKey[i] ^ ((r+1)*0x1F1F)) + r;
        }
    }
    return roundKeys;
}

// ------------------- Encryption ------------------- //

array<uint16_t, 8> encryptBlock(array<uint16_t, 8> block, const array<array<uint16_t, 8>, 10>& roundKeys, int blockIndex) {
    for(int r=0; r<10; r++){
        // Layer A: Bit permutation
        array<int, 128> perm = generatePermutation(0xDEADBEEF ^ r, blockIndex);
        array<uint16_t, 8> permuted = {0};
        for(int i=0; i<128; i++){
            int fromBit = i;
            int toBit = perm[i];
            if(block[fromBit/16] & (1 << (fromBit%16))) {
                permuted[toBit/16] |= (1 << (toBit%16));
            }
        }
        block = permuted;

        // Layer B: Nonlinear mixing
        nonlinearMix(block, roundKeys[r]);

        // Layer C: XOR + rotation
        for(int i=0; i<8; i++){
            block[i] = rotateLeft16(block[i], roundKeys[r][i] % 16) ^ roundKeys[r][i];
        }
    }
    return block;
}

// ------------------- Decryption (inverse layers) ------------------- //
array<uint16_t, 8> decryptBlock(array<uint16_t, 8> block, const array<array<uint16_t, 8>, 10>& roundKeys, int blockIndex) {
    for(int r=9; r>=0; r--){
        // Layer C: inverse XOR + rotation
        for(int i=0; i<8; i++){
            block[i] = rotateLeft16(block[i] ^ roundKeys[r][i], 16 - (roundKeys[r][i] % 16));
        }

        // Layer B: inverse nonlinear mixing (modular inverse)
        const uint16_t Q = 65537;
        for(int i=7; i>=0; i--){
            uint16_t nextSeg = (i==7) ? block[0] : block[i+1];
            uint16_t inv = 1; // simple placeholder: modular inverse calculation needed for real implementation
            // For demo purposes, skipping exact modular inverse
            block[i] = block[i]; // placeholder
        }

        // Layer A: inverse permutation
        array<int, 128> perm = generatePermutation(0xDEADBEEF ^ r, blockIndex);
        array<int, 128> invPerm;
        for(int i=0; i<128; i++) invPerm[perm[i]] = i;

        array<uint16_t, 8> permuted = {0};
        for(int i=0; i<128; i++){
            int fromBit = i;
            int toBit = invPerm[i];
            if(block[fromBit/16] & (1 << (fromBit%16))) {
                permuted[toBit/16] |= (1 << (toBit%16));
            }
        }
        block = permuted;
    }
    return block;
}

// ------------------- Demo ------------------- //

int main() {
    // Example master key: 8x16-bit segments
    array<uint16_t,8> masterKey = {0x1234,0x5678,0x9ABC,0xDEF0,0x1111,0x2222,0x3333,0x4444};

    // Expand keys
    auto roundKeys = expandKey(masterKey);

    // Example plaintext block (8x16-bit)
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