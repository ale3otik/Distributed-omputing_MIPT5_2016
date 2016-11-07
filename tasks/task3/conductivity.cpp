#include <mpi.h> 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <algorithm> 
#include <assert.h>

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
*
* TIME = 50 * (60 / dt) = 3000 / dt
* assume dt = 1e-6 => TIME = 3 * 1e9
*
* We would use that u[i][j] = u[i][j + 1] for all j (because of equation or just symmetry).
* Therefore find out u[i][j] = v[i][j] from equations.
*
*/

const double dt = 1e-6;

double conduct_factor;
double recalc_coef;
int len_x;
int len_y;

double * u_next, * u_last;

int proc_qnt;
int rank;
int my_task_len;

double lbound;
double rbound;

MPI_Request request_left;
MPI_Request request_right;

void precalc_constants() {
    conduct_factor = (double)lambda / (double)(pho * cup_conduct);
    len_x = (int)(LX / h);
    recalc_coef = conduct_factor * dt / (h * h);
}

double crd(int i) {
    return (double)(i + 1) * h - h/2.0;
}

double get_u(int i , double * vals) {
    if(i < 0) return Ul;
    if(i > len_x - 1) return Ur;
    return vals[i];
}

double calc_u_i(int i){
    double ui = get_u(i,u_last);
    double ui1 = get_u(i + 1,u_last);
    double ui2 = get_u(i - 1,u_last);
    return ui + recalc_coef * (ui1 + ui2 - 2.0 * ui);
}

void send_bounds() {
    if(rank != 0) {
        MPI_Isend(u_last, 1, MPI_DOUBLE, rank - 1, RIGHT_BOUND_ID_RECV, MPI_COMM_WORLD, NULL);
    }

    if(rank < proc_qnt - 1) {
        MPI_Isend(u_last + my_task_len - 1, 1, MPI_DOUBLE, rank + 1, LEFT_BOUND_ID_RECV, MPI_COMM_WORLD, NULL);
    }
}

void recv_bounds() {
    if(rank == proc_qnt - 1) {
        rbound = Ur;
    } else {
        MPI_Irecv(&rbound, 1, MPI_DOUBLE, rank + 1, RIGHT_BOUND_ID_RECV, MPI_COMM_WORLD, &request_right);
    }
    
    if(rank == 0) {
        lbound = Ul;
    } else {
        MPI_Irecv(&lbound, 1, MPI_DOUBLE, rank - 1, LEFT_BOUND_ID_RECV, MPI_COMM_WORLD, &request_left);
    }
}

void calc() {
    send_bounds();
    recv_bounds();
    for(int i = 1; i < my_task_len - 1; ++i) {
        u_next[i] = calc_u_i(i);
    }

    if(rank != proc_qnt - 1) {
        MPI_Wait(&request_right, NULL);
    }

    if(rank != 0) {
        MPI_Wait(&request_left, NULL);
    }

    u_next[0] = calc_u_i(0);
    u_next[my_task_len - 1] = calc_u_i(my_task_len - 1); // if 0 == my_task_len - 1 it's OK

    std::swap(u_last, u_next);
}

void send_result() {
    MPI_Send(u_last, my_task_len, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
}

double * recv_result() {
    double * result = (double * )malloc(sizeof(double) * len_x);
    assert(result > 0);
    memcpy(result, u_last, sizeof(double) * my_task_len);
    for(int i = 1; i < proc_qnt; ++i) {
        int len = my_task_len + (i == rank - 1 ? len_x % proc_qnt : 0);
        MPI_Recv(result + i * my_task_len, len, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, NULL);
    }
    return result;
}

int main(int argc , char ** argv) {
    MPI_Init(&argc,&argv);
    precalc_constants();
    MPI_Comm_size(MPI_COMM_WORLD, &proc_qnt);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    proc_qnt = std::min(proc_qnt, len_x);
    my_task_len = len_x / proc_qnt;
    if(rank == proc_qnt - 1) {
        my_task_len += len_x % proc_qnt;
    }

    u_next = (double*) malloc(my_task_len * sizeof(double));
    u_last = (double*) malloc(my_task_len * sizeof(double));
    assert(u_next > 0);
    assert(u_last > 0);

    for(int i = 0; i < my_task_len; ++i) {
        u_last = U0;
    }

    double eps = 1e-12;
    for(double tm = dt; tm <= 60.0 + eps; tm += dt) {
        calc();
    }

    if(rank == 0) {
        float * result = recv_result();
        for(int i = 0; i < len_x; ++i) {
            printf("%lf, ",result[i]);
        }
        printf("\n");

        for(int i = 0; i < len_x; ++i) {
            printf("%lf, ", crd(i));
        }
        printf("\n");

        free(result);
    } else {
        send_result();
    }
    free(u_last);
    free(u_next);
    MPI_Finalize();
    return 0;
}