import random
from tqdm import tqdm
from statistics import mean, variance
import math
import numpy as np

def sphere(x):
    result = 0
    for elem in x:
        result += elem**2
    return result

def rastrigin(x):
    result = 0
    for elem in x:
        result += (elem**2 - 10 * math.cos(2 * math.pi * elem) + 10)
    return result

def next_v(a, b, c):
    Fw = 0.5
    return a + Fw * (b - c)

def extract_three(i, x):
    while True:
        tmp = random.sample(x, 3)
        if i not in tmp: break
    return tmp

def myDE(D, func):
    M = 30
    t_max = 1000
    Cr = 0.9
    t = 0
    f_end = 1**-5
    x_min = -5
    x_max = 5
    x = [[random.uniform(x_max,x_min) for i in range(D)] for j in range(M)] 
    x_new = [[0 for i in range(D)] for j in range(M)] 
    v = [0 for i in range(D)] 
    u = [0 for i in range(D)]
    f = [func(i) for i in x]
    f_tmp = 0
    f_best = float('inf')
    x_best = [0 for i in range(D)]
    
    while t < t_max:
        t = t + 1
        for i in range(M):
            a, b, c = extract_three(i, x)
            for d in range(D):
                v[d] = next_v(a[d], b[d], c[d])
            Jr = random.randrange(D)
            for d in range(D):
                ri = random.random()
                if ri < Cr or d == Jr:
                    u[d] = v[d]
                else:
                    u[d] = x[i][d]
            f_tmp = func(u)
            if f_tmp < f[i]:
                f[i] = f_tmp
                for d in range(D):
                    x_new[i][d] = u[d]
                if f_tmp < f_best:
                    f_best = f_tmp
                    for d in range(D):
                        x_best[d] = u[d]
            else:
                for d in range(D):
                    x_new[i][d] = x[i][d]
        for i in range(M):
            for d in range(D):
                x[i][d] = x_new[i][d]
        if f_best < f_end:
            break
    return t, f_best, x

def simulation(D,func):
    time = []
    f_value = []
    x_position = []
    pbar = tqdm(total=100)
    for i in range(100):
        t, f, x = myDE(D, func)
        time.append(t)
        f_value.append(f)
        x_position.append(x)
        pbar.update(1)
    pbar.close()
    std = np.array(f_value)
    print(D, func.__name__, mean(f_value), np.var(std,ddof=0), np.var(std,ddof=1), mean(time))

if __name__ == "__main__":
    print("D, function, f-value mean, f-value var, f-value std-var, loop time mean")
    simulation(2, sphere)
    simulation(5, sphere)
    simulation(20, sphere)

    simulation(2, rastrigin)
    simulation(5, rastrigin)
    simulation(20, rastrigin)