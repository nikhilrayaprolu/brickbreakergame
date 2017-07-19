x=[0 0 1 1 1 0 0];
y=[0 1 2 3 2 1 0];
z=conv(x,y)

figure
subplot(3,1,1);
hist(x);

subplot(3,1,2);
hist(y);

subplot(3,1,3);
hist(z);