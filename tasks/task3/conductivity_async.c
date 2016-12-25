#include <mpi.h> 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h> 
#include <assert.h>
#include <math.h>
const double T = 60;
const double h = 0.01; 
const double LX = 0.5;
const double LY = 0.5;
const double Ul = 80;
const double Ur = 30;
const double U0 = 5;
const int rho = 8960;
const int cup_conduct = 380;
const int lambda = 401;

const int RIGHT_BOUND_ID_RECV = 1111;
const int LEFT_BOUND_ID_RECV = 2222;
/* 
* TIME = 50 * (60 / dt) = 3000 / dt
* assume dt = 1e-6 => TIME = 3 * 1e9
* dt should be small to sequence be convergence 
*/

const double dt = 1e-3;

double conduct_factor;
double recalc_coef;
int len_x;
int len_y;

double ** u_next, ** u_last;
double ** v_cur;

int proc_qnt;
int rank;
int my_task_len_x;
int my_task_len_y;

double * lbound;
double * rbound;

MPI_Status Status;
MPI_Request request_left;
MPI_Request request_right;

void precalc_constants() {
    conduct_factor = (double)lambda / (double)(rho * cup_conduct);
    len_x = (int)(LX / h);
    recalc_coef = conduct_factor * dt / (h * h);
}

double crd(int i) {
    return (double)(i + 1) * h - h/2.0;
}

double get_u(int i , int j, double ** vals) {
	if(i < 0){
		return lbound[j];
	}
	if(i > my_task_len_x - 1) {
		return rbound[j];
	}

    return vals[i][j];
}

double get_v(int i , int j) {
	if(j < 0) return v_cur[i][0];
	if(j >= my_task_len_y) return v_cur[i][my_task_len_y - 1];
	return v_cur[i][j];	
}

double calc_v_ij(int i, int j) {
//	printf("<%d>\n",i);
	double uij = get_u(i,j,u_last);
	double u1 = get_u(i+1,j,u_last);
	double u2 = get_u(i-1,j,u_last);
	return uij + recalc_coef * (u1 + u2 - 2.0 * uij);
}

double calc_u_ij(int i, int j){
   	double vij = get_v(i,j);
	double v1 = get_v(i,j+1);
	double v2 = get_v(i,j-1);
	double res = vij + recalc_coef * (v1 + v2 - 2.0 * vij);
	return res; 
	
}

MPI_Request request_send_0;
MPI_Request request_send_1;
void send_bounds() {
    if(rank != 0) {
        MPI_Isend(*u_last, my_task_len_y, MPI_DOUBLE, rank - 1, RIGHT_BOUND_ID_RECV, MPI_COMM_WORLD, &request_send_0);
    }

    if(rank < proc_qnt - 1) {
        MPI_Isend(*(u_last + my_task_len_x - 1), my_task_len_y, MPI_DOUBLE, rank + 1, LEFT_BOUND_ID_RECV, MPI_COMM_WORLD, &request_send_1);
    }
}

void recv_bounds() {
    if(rank == proc_qnt - 1) {
	for(int i = 0; i < my_task_len_y;++i)
       		rbound[i] = Ur;
    } else {
        MPI_Irecv(rbound,my_task_len_y, MPI_DOUBLE, rank + 1, RIGHT_BOUND_ID_RECV, MPI_COMM_WORLD, &request_right);
    }
    
    if(rank == 0) {
	
	for(int i = 0; i < my_task_len_y;++i)
       		lbound[i] = Ul;
    } else {
        MPI_Irecv(lbound, my_task_len_y, MPI_DOUBLE, rank - 1, LEFT_BOUND_ID_RECV, MPI_COMM_WORLD, &request_left);
    }
}

void calc() {
    send_bounds();
    recv_bounds();

	for(int i = 1; i < my_task_len_x-1; ++i) {
		for(int j = 0; j < my_task_len_y;++j) {

			v_cur[i][j] = calc_v_ij(i,j);

		}
	}

	for(int i = 1; i < my_task_len_x-1; ++i) {
		for(int j = 0; j < my_task_len_y;++j) {
        		u_next[i][j] = calc_u_ij(i,j);
    		}
	}

	

    if(rank != proc_qnt - 1) {
	MPI_Wait(&request_send_1,&Status);
        MPI_Wait(&request_right, &Status);
    }

    if(rank != 0) {
    	MPI_Wait(&request_send_0,&Status);
        MPI_Wait(&request_left, &Status);
    }
	
    	for(int j = 0; j < my_task_len_y;++j){
		v_cur[0][j] = calc_v_ij(0,j);
		v_cur[my_task_len_x-1][j] = calc_v_ij(my_task_len_x-1,j);
   		u_next[0][j] = calc_u_ij(0,j);
   	 	u_next[my_task_len_x - 1][j] = calc_u_ij(my_task_len_x - 1,j); // if 0 == my_task_len - 1 it's OK
	}
	double ** tmp = u_last;
	u_last = u_next;
	u_next = tmp;
}

void send_result() {
	for(int i = 0; i < my_task_len_x; ++i) {
   		MPI_Send(u_last[i], my_task_len_y, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
	}
}

double ** recv_result() {
    double ** result = (double **)malloc(sizeof(double*) * len_x);
	for(int i = 0; i < len_x; ++i) {
		result[i] = (double*)malloc(sizeof(double) * len_x);
	}

    assert(result > 0);
	int k = 0;
	for(;k < my_task_len_x;++k) {
   		memcpy(result[k], u_last[k], sizeof(double) * my_task_len_y);
	}
    for(int i = 1; i < proc_qnt; ++i) {
        int len = my_task_len_x + (i == proc_qnt - 1 ? len_x % proc_qnt : 0);
	for(int m = 0; m < len; ++m) {
        	MPI_Recv(result[k], my_task_len_y, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &Status);
   		++k;	
	}
    }
    return result;
}

int main(int argc , char ** argv) {
    MPI_Init(&argc,&argv);
    double  begin = MPI_Wtime();
    precalc_constants();
    MPI_Comm_size(MPI_COMM_WORLD, &proc_qnt);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(proc_qnt > len_x) {
		proc_qnt = len_x;
	}
    
    if(rank >= proc_qnt) {
	MPI_Finalize();
	return 0;
	}
   	my_task_len_y = len_x;
	my_task_len_x = len_x / proc_qnt;;
    if(rank == proc_qnt - 1) {
        my_task_len_x += len_x % proc_qnt;
    }

	lbound = (double*) malloc(sizeof(double) * my_task_len_y);
	rbound = (double*)malloc(sizeof(double) * my_task_len_y);
	u_next = (double **) malloc(my_task_len_x * sizeof(double*));
   	u_last = (double **) malloc(my_task_len_x * sizeof(double*));
   	v_cur = (double**)malloc(my_task_len_x * sizeof(double));
	
   	assert(u_next > 0);
   	assert(u_last > 0);
	assert(v_cur > 0);
	assert(lbound > 0);
	assert(rbound > 0);
	
    for(int i = 0; i < my_task_len_x; ++i) {
	v_cur[i] = (double*) malloc(my_task_len_y * sizeof(double));
	u_next[i] = (double *) malloc(my_task_len_y * sizeof(double));
	u_last[i] = (double *) malloc(my_task_len_y * sizeof(double));
	for(int j = 0; j < my_task_len_y; ++j) {
		u_last[i][j] = U0;
	}
    }    

    double eps = 1e-12;
    for(double tm = dt; tm <= 60.0 + eps; tm += dt) {
        calc();
    }

    if(rank == 0) {
        double ** result = recv_result();
   	double end = MPI_Wtime();
       
/*	
	double sum = 0;
	for(int i = 0; i < len_x; ++i) {
		sum += result[i][0];
           	printf("%lf, ",result[i][0]);
        }
        printf("\n");
	printf("<<%lf>>\n",sum);
	
        for(int i = 0; i < len_x; ++i) {
            printf("%lf, ", crd(i));
        }
	printf("\n");	
*/
	printf("%lf, ", end - begin); 
	for(int i = 0; i < len_x; ++i) free(result[i]);
    free(result);
    } else {
        send_result();
    }

	for(int i = 0; i< my_task_len_x;++i) {
		free(u_next[i]);
		free(u_last[i]);
		free(v_cur[i]);
	}
	
   	free(u_last);
   	free(u_next);
	free(v_cur);
	free(rbound);
	free(lbound);
    MPI_Finalize();
    return 0;
}
