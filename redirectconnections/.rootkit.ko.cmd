cmd_/home/zac/COSC439Project/redirectconnections/rootkit.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000  --build-id  -T ./scripts/module-common.lds -o /home/zac/COSC439Project/redirectconnections/rootkit.ko /home/zac/COSC439Project/redirectconnections/rootkit.o /home/zac/COSC439Project/redirectconnections/rootkit.mod.o;  true
