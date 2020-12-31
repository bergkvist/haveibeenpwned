ORDERED_PASSWORDS := pwned-passwords-sha1-ordered-by-hash-v7
SHA1_BINARY_FILE := data/pwned-sha1
COUNT_BINARY_FILE := data/pwned-count
BINARY_FILES := $(SHA1_BINARY_FILE) $(COUNT_BINARY_FILE)
HASH := FFFFFFFEE791CBAC0F6305CAF0CEE06BBE131160

lookup: ./dist/binary-lookup $(BINARY_FILES)
	$< $(HASH) $(BINARY_FILES)

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
	gcc $^ -o $@ -O3