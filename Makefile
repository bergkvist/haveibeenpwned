ORDERED_PASSWORDS := pwned-passwords-sha1-ordered-by-hash-v7
SHA1_BINARY_FILE := data/pwned-sha1
COUNT_BINARY_FILE := data/pwned-count
BINARY_FILES := $(SHA1_BINARY_FILE) $(COUNT_BINARY_FILE)
HASH := FFFFFFFEE791CBAC0F6305CAF0CEE06BBE131160

lookup: ./dist/binary-lookup ./dist/sha1 $(BINARY_FILES)
	@if [ -z $(PASSWORD) ]; then \
		($< $(HASH) $(BINARY_FILES));\
	else \
		(./dist/sha1 $(PASSWORD) | xargs -i $< {} $(BINARY_FILES));\
	fi
	
read-sha1: ./dist/read-sha1-index $(SHA1_BINARY_FILE)
	$< $(INDEX) $(SHA1_BINARY_FILE)	

$(BINARY_FILES) &: ./dist/create-binary-files data/$(ORDERED_PASSWORDS).txt
	# Process txt file to split into binary columns
	$< data/$(ORDERED_PASSWORDS).txt $(BINARY_FILES)

data/$(ORDERED_PASSWORDS).txt: data/$(ORDERED_PASSWORDS).7z
	# Extract ordered password txt from 7zip archive
	7z x -odata $<

data/$(ORDERED_PASSWORDS).7z:
	# Download 7zip archive with pwned passwords
	wget -P data/ https://downloads.pwnedpasswords.com/passwords/$(ORDERED_PASSWORDS).7z

watch:
	ls src/*.c | entr -sc 'make lookup'

dist/%: src/%.c
	mkdir -p dist
	gcc $^ -o $@ -O3 -lssl -lcrypto

watch-test-pwnedlookup:
	ls src/python-binding.c test.py Makefile | entr -sc 'make test-pwnedlookup'

test-pwnedlookup: dist/pwnedlookup.so test.py
	python test.py

dist/pwnedlookup.so: src/python-binding.c
	gcc -shared -O3 -lssl -lcrypto -fPIC $< -o $@ -I/usr/include/python3.8