defmodule Falcon1024Test do
  use ExUnit.Case
  doctest Falcon1024

  @module Falcon1024
  @seed_size 48
  @pub_size 1793
  @priv_size 2305

  test "gen_keys" do
    {:ok, pub, priv} = @module.gen_key_pair()
    assert byte_size(pub) == @pub_size, "pubkey no has #{@pub_size} bytes"
    assert byte_size(priv) == @priv_size, "privkey no has #{@priv_size} bytes"

    IO.puts("pub_size #{byte_size(pub)} bytes")
    IO.puts("priv_size #{byte_size(priv)} bytes")
  end

  test "seed" do
    seed = :crypto.strong_rand_bytes(@seed_size)
    {:ok, pub, priv} = @module.gen_keys_from_seed(seed)
    assert byte_size(pub) == @pub_size, "pubkey no has #{@pub_size} bytes"
    assert byte_size(priv) == @priv_size, "privkey no has #{@priv_size} bytes"

    IO.puts("seed_size #{@seed_size}")
    IO.puts("pub_size #{byte_size(pub)} bytes")
    IO.puts("priv_size #{byte_size(priv)} bytes")
  end

  test "gen pub from privkey" do
    {:ok, pub, priv} = @module.gen_key_pair()
    {:ok, pubk} = @module.gen_pub_key(priv)
    assert pubk == pub, "expected pubkey same pubkey from privkey"

    IO.puts("pub_size #{byte_size(pub)} bytes")
    IO.puts("priv_size #{byte_size(priv)} bytes")
  end

  test "sign and verify" do
    msg = "Hello world!"
    {:ok, pub, priv} = @module.gen_key_pair()
    {:ok, sig} = @module.sign(priv, msg)
    r = @module.verify(msg, sig, pub)
    assert r == :ok, "Error verify signature"

    IO.puts("signature_size #{byte_size(sig)}")
    IO.puts("verification = #{r}")
  end

  test "invalid sign and verify" do
    msg = "Hello world!"
    {:ok, pub, priv} = @module.gen_key_pair()
    {:ok, sig} = @module.sign(priv, msg)
    invalid_sig = :rand.bytes(byte_size(sig))
    r = @module.verify(msg, invalid_sig, pub)
    assert r != :ok
  end
end
