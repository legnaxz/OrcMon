# GNU Make solution makefile autogenerated by Premake
# Usage: make [ config=config_name ]
# Where {config_name} is one of: "debug", "release".

ifndef config
  config=debug
endif
export config

PROJECTS := OrcMon

.PHONY: all clean $(PROJECTS)

all: $(PROJECTS)

OrcMon: 
	@echo ==== Building OrcMon ====
	@${MAKE} --no-print-directory -C . -f watcher_demo.make

clean:
	@${MAKE} --no-print-directory -C . -f watcher_demo.make clean
