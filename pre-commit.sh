#!/bin/bash
gzip -vrkf dev_data/*.htm
gzip -vrkf dev_data/css/*.css
gzip -vrkf dev_data/js/*.js
gzip -vrkf dev_data/lang/lang.*.json

mv dev_data/*.htm.gz data/
mv dev_data/js/*.js.gz data/js
mv dev_data/css/*.css.gz data/css
mv dev_data/lang/*.json.gz data/lang

#test -f *.generic.bin && cp *.generic.bin build/build.generic_flash_size_1Mb.254Kb_`date '+%G.%m.%d'`.bin
FILE_SUM=`md5sum jaluzi.ino.generic.bin`
COPY_SUM=`md5sum ./build/$(ls -lt ./build | head -n2 |tail -n1 | awk '{print $9}')`
if [ "$FILE_SUM" != "$COPY_SUM" ]
then
cp jaluzi.ino.generic.bin build/build.0x00000_flash_size_1Mb.256Kb_`date '+%G.%m.%d'`.bin
fi


cp /tmp/buil*.spiffs/jaluzi.spiffs.bin ./build/spiffs.0xBB000_flash_size_1Mb.256Kb_`date '+%G.%m.%d'`.bin
