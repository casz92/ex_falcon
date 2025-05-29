defmodule Mix.Tasks.Make.All do
  use Mix.Task

  @shortdoc "Runs both Makefiles"

  def run(_) do
    # Mix.shell().cmd("make -f Makefile")
    Mix.shell().cmd("make -f Makefile1024")
  end
end
