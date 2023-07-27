#include <stdio.h>

int main(){
    FILE *f = fopen("test_3.bat", "w");
    for(int i=1; i<=100; i++){
        fprintf(f, "main_3.exe < ../tests/archivio_test_aperti/open_%d.txt > ../tests/archivio_test_aperti/esiti/open_%d.output.txt\n", i, i);
    }

    f = fopen("diff.bat", "w");
    for(int i=1; i<=100; i++){
        fprintf(f, "fc open_%d.output.txt esiti/open_%d.output.txt > nul\n", i, i);
        fprintf(f, "if %%errorlevel%% neq 0 (echo %d)\n", i);
    }
}