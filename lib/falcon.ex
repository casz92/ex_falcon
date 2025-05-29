defmodule Falcon do
  @compile {:autoload, false}
  @on_load {:load_nif, 0}

  def load_nif do
    path = :filename.join(:code.priv_dir(:ex_falcon), ~c"falcon_nif")
    :erlang.load_nif(path, 0)
  end

  @spec gen_key_pair() :: {:ok, binary(), binary()} | {:error, atom()}
  def gen_key_pair do
    raise "Function gen_key_pair is not implemented!"
  end

  @spec gen_pub_key(binary()) :: {:ok, binary()} | {:error, atom()}
  def gen_pub_key(_) do
    raise "Function gen_pub_key is not implemented!"
  end

  @spec gen_keys_from_seed(binary()) :: {:ok, binary(), binary()} | {:error, atom()}
  def gen_keys_from_seed(_) do
    raise "Function gen_keys_from_seed is not implemented!"
  end

  @spec sign(binary(), binary()) :: {:ok, binary()} | {:error, atom()}
  def sign(_, _) do
    raise "Function sign is not implemented!"
  end

  @spec verify(binary(), binary(), binary()) :: :ok | :error
  def verify(_, _, _) do
    raise "Function sign is not implemented!"
  end

  def pubkey_size, do: 897
  def secret_key_size, do: 1281
end
