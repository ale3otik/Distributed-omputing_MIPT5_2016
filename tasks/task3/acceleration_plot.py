import matplotlib.pyplot as plt
x = [i for i in range(1,13)]
t = [13.120541, 7.893350, 5.413650, 4.386797, 3.720060, 3.327431,\
 2.685412, 2.702376, 3.283919, 1.981320, 3.267494, 2.15503]
t2 = [13.445650, 6.891670, 5.686936, 4.660783, 3.604818, 3.606095,\
  2.818223, 2.963527, 3.413399, 2.103339, 3.307905, 2.153178]
acc = [t[0] / t_i for t_i in t]
acc2 = [t[0] / t_i for t_i in t2]
plt.figure(figsize=(15,10))
plt.grid(True)
plt.title('Acceleration(num processors)')
plt.xlabel('n')
plt.ylabel('Acceleration')
plt.plot([1,12],[1,12],'--',color='black',label='best expected')
plt.plot(x,acc,'-o',color='red',label='real_unblocked')
plt.plot(x,acc2,'-o',color='green',label='real_blocked')
plt.legend(loc='best')
plt.savefig("acceleration.jpg")
plt.xlim(1,12);
plt.ylim(1,12);
plt.show()

