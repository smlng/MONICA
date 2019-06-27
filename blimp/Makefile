APPLICATION = monica-blimp
RIOTBASE ?= ../../RIOT

BOARD ?= samr21-xpro
BOARD_WHITELIST := samr21-xpro b-l072z-lrwan1

# mandatory modules
USEMODULE += fmt
USEMODULE += isrpipe
USEMODULE += isrpipe_read_timeout
USEMODULE += xtimer
# networking modules
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_sock_udp
# optional modules, e.g. for debugging
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gcoap

ifneq (,$(filter samr21-xpro,$(BOARD)))
  #USEMODULE += gcoap
else
  USEMODULE += sx1276
  USEPKG += semtech-loramac
endif

DEVELHELP=1

include $(RIOTBASE)/Makefile.include
