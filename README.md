# Falcon

Falcon: It's a post-quantum cryptography library for digital signatures based on the Falcon algorithm.

[Official Website](https://falcon-sign.info)

## Introduction

Falcon is a post-quantum digital signature scheme selected by **NIST** as part of its efforts to develop cryptographic standards resilient to quantum attacks. It is based on **NTRU lattices** and offers an optimal balance between security and efficiency.

## Features

- **Post-quantum security:** Based on the **Quantum Random Oracle** model.
- **Optimized efficiency:** Compact key and signature sizes.
- **Elixir implementation via NIF:** Native C code integration for optimal performance.
- **Compatible with Falcon-512 and Falcon-1024**.

## Signature Padding Optimization

Falcon signatures include an initial **41-byte padding**, which is automatically reconstructed during the verification process. This means the padding can be safely removed when storing or transmitting signatures, reducing their overall size without affecting security or validity.

### Key Considerations:
- **Storage & Transmission Efficiency**: Removing the padding reduces signature size while maintaining integrity.
- **Compatibility**: Since the verification process restores the removed bytes, there is no impact on the correctness of signature validation.
- **Implementation Details**: Ensure the padding is removed consistently, and validate that the verification method correctly regenerates it.

This optimization offers a practical way to minimize storage and transmission overhead while preserving Falcon’s cryptographic strength.

### Compilation & Testing
- **Compiled and tested on Linux x64.**

### Details
| Security | LOGN |
|----------|------|
| Falcon-512 | 9 |
| Falcon-1024 | 10 |

### Key and Signature Sizes

#### Falcon-512
| Component | Size (bytes) |
|-----------|--------------|
| Public key | 897 |
| Private key | 1281 |
| Signature | ~625 (666) |

> **Note:** The maximum signature size is 625 bytes, as the first 41 characters <<57, 0, 0, 0...>> are removed.

#### Falcon-1024
| Component | Size (bytes) |
|-----------|--------------|
| Public key | 1793 |
| Private key | 2305 |
| Signature | ~1239 (1280) |

> **Note:** The maximum signature size is 1239 bytes, as the first 41 characters <<58, 0, 0, 0...>> are removed.

## Installation & Compilation

Before compiling, make sure to install `build-essential` and `erlang-dev`.

```bash
mix deps.get
mix compile
```

## Usage
To add the library to your project, include the following dependency in mix.exs:
def deps do
  [
    {:ex_falcon, "~> 0.1.0"}
  ]
end

### Example
```elixir
msg = "Hello world!"
{:ok, pubkey, privkey} = Falcon.gen_key_pair()
{:ok, signature} = Falcon.sign(privkey, msg)
:ok = Falcon.verify(msg, signature, pubkey)

# Generate from seed
{:ok, pubkey, privkey} = Falcon.gen_keys_from_seed("your_seed")
# Generate public key from private key
{:ok, pubkey2} = Falcon.gen_pub_key(privkey)
pubkey == pubkey2

# Falcon1024 test
{:ok, pub, priv} = Falcon1024.gen_key_pair()
{:ok, signature} = Falcon1024.sign(priv, msg)
:ok = Falcon1024.verify(msg, signature, pub)
```

## Testing
Run the following commands to test Falcon-512 and Falcon-1024:
mix run test/falcon_test.exs
mix run test/falcon_1024_test.exs
