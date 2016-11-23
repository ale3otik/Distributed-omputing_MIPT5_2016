#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <assert.h>
#include <memory.h>

#include <omp.h>

int nthr;
int ** get_allocated_field(int x_len, int y_len) {
    int ** field = (int **) malloc(sizeof(int*)*x_len);
    for(int i = 0; i < x_len; ++i) {
        field[i] = (int *) malloc(sizeof(int) * y_len);
        memset(field[i],0,sizeof(int) * y_len);
    }
    return field;
}

void copy_field(int ** dst, int ** source,int x_len,int y_len) {
    for(int i = 0; i < x_len; ++i) {
        memcpy(dst[i],source[i],sizeof(int) * y_len);
    }
}

int count_environment(int x, int y, int ** field, int x_len, int y_len) {
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
int ** calc_life(int ** field,int x_len,int y_len,long long game_duration) {
    int ** new_field = get_allocated_field(x_len,y_len);
    for(long long t = 0; t < game_duration; ++t) {
        #pragma omp parallel num_threads(nthr)
        {
            #pragma omp collapse(2) 
            {
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
                                new_field[x][y] = 1;
                            } else {
                                new_field[x][y] = 0;
                            }
                        }
                    }
                }    
            }
        }
        int ** tmp = new_field;
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
    long long game_duration = 1e3;
    FILE * ftime = fopen("time.txt","w");
    for(nthr = 1; nthr < 13; ++nthr) {
        // omp_set_num_threads(nthr);
        int x_len = -1,y_len = -1;
        int ** field; // x // y
        
        FILE *fin = fopen("state.dat","r");
        FILE *fout = fopen("life.dat","w");
        assert(fin > 0 && fout > 0);
        fscanf(fin,"%d%d",&x_len,&y_len);
        field = get_allocated_field(x_len,y_len);
        
        printf("%d %d\n",x_len,y_len);
        while(!feof(fin)) {
            int x,y;
            fscanf(fin,"%d%d",&x,&y);
            if(feof(fin)) break;
            --x; --y;
            field[x][y] = 1;
        }
        fclose(fin);
        
        double begin = omp_get_wtime();
        field = calc_life(field,x_len,y_len,game_duration);
        double end = omp_get_wtime();
        printf("nthreads = %d, time elapsed : %lf",nthr,end-begin);
        fprintf(ftime,"%lf, ",end-begin);
        fprintf(fout, "%d,%d\n",x_len,y_len);
        for(int x=0; x < x_len; ++x) {
            for(int y=0; y < y_len; ++y) {
                if(field[x][y]) {
                    fprintf(fout, "%d %d\n",x+1,y+1);
                }
            }    
        }
        fclose(fout);

        for(int i = 0; i < x_len; ++i) {
            free(field[i]);
        }
        free(field);
    }
    fclose(ftime);
    return 0;
}
