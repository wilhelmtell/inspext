all:
	@./waf build

check:
	@./waf check

dist:
	@./waf dist

distcheck:
	@./waf distcheck

clean:
	@./waf clean

distclean:
	@./waf distclean

install:
	@./waf install

uninstall:
	@./waf uninstall
