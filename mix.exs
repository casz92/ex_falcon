defmodule Falcon.MixProject do
  use Mix.Project

  @version "0.1.1"

  def project do
    [
      app: :ex_falcon,
      version: @version,
      elixir: "~> 1.3",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      aliases: [compile: ["make.all"]],
      deps: deps(),
      package: package()
    ]
  end

  defp deps do
    [
      {:elixir_make, "~> 0.4", runtime: false},
      {:ex_doc, "~> 0.31", only: :dev, runtime: false}
    ]
  end

  defp package do
    [
      name: "ex_falcon",
      description:
        "A post-quantum cryptography library for digital signatures based on the Falcon algorithm",
      files: ["lib", "c_src", "mix.exs", "README*", "libfalcon", "Makefile", "Makefile1024", "LICENSE*"],
      maintainers: ["Carlos Suarez"],
      links: %{"GitHub" => "https://github.com/casz92/ex_falcon"},
      licenses: ["MIT"]
    ]
  end
end
