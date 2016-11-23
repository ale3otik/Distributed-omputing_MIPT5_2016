#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <file.h>
#include <assert.h>
#include <omp.h>

short ** get_allocated_field(int x_len, int y_len) {
    field = (short **) malloc(sizeof(short*)*x_len);
    for(int i = 0; i < x_len; ++i) {
        field[i] = (short *) malloc(sizeof(short) * y_len);
        memset(field[i],0,sizeof(field[i]));
    }
    return field;
}

copy_field(short ** dst, short ** source,int x_len,int y_len) {
    for(int i = 0; i < x_len; ++i) {
        memcpy(dst[i],source[i],sizeof(source[i]));
    }
}

int count_environment(int x, int y, const short ** field, int x_len, int y_len) {
    int x_dir[] = {(x - 1 + x_len) % x_len , (x + 1) % x_len , x}; 
    int y_dir[] = {(y - 1 + y_len) % y_len , (y + 1) % y_len , y};
    int ans = 0;
    for(int ix = 0 ;ix < 3; ++ix) {
        for(int iy = 0; iy < 3; ++iy) {
            if(iy == 2 && ix == 2) continue;
            ans += field[x_dir[ix]][y_dir[iy]];
        }   
    }
    return ans;
}
short ** calc_life(short ** field,int x_len,int y_len,long long game_duration) {
    new_field = get_allocated_field(x_len,y_len);
    for(long long t = 0; t < game_duration; ++t) {
        
        for(int x = 0; x < x_len; ++x) {
            for(int y = 0; y < y_len; ++y) {
                int env = count_environment(x,y,field,x_len,y_len);
                if(field[x][y]) {
                    if(env == 2 || env == 3) {
                        new_field[x][y] = 1;
                    } else {
                        new_field[x][y] = 0;
                    }
                } else {
                    if(env == 3) {
                        new_field[x] = 1;
                    } else {
                        new_field[y] = 0;
                    }
                }
            }
        }    
        
        short ** tmp = new_field;
        new_field = field;
        field = tmp;

    }

    for(int i = 0; i < x_len; ++i) {
        free(new_field[i]);
    }
    free(new_field);
    
    return field;
}

int main() {
    long long game_duration = 1e7;
    int x_len = -1,y_len = -1;
    short ** field; // x // y
    FILE *fin = fopen("state.dat");
    assert(fin > 0 && fout > 0);
    fscanf(fin,"%d",&y_len);
    fgetc(fin);
    fscanf(fin,"%d",&y_len);
    field = get_allocated_field(x_len,y_len);

    while(!feof(fin)) {
        int x,y;
        fscanf(fin,"%d",&x);
        fgetc(fin);
        fscanf(fin,"%d",&y);
        --x; --y;
        field[x][y] = 1;
    }
    fclose(fin);
    
    field = calc_life(field,x_len,y_len,game_duration);

    FILE *fout = fopen("life.dat");
    fprintf(fout, "%d,%d\n",x_len,y_len);
    for(int x=0; x < x_len; ++x) {
        for(int y=0; y < y_len; ++y) {
            if(field[x][y]) {
                fprintf(fout, "%d,%d\n",x,y);
            }
        }    
    }
    fclose(fout);

    for(int i = 0; i < x_len; ++i) {
        free(field[i]);
    }
    free(field);
    return 0;
}