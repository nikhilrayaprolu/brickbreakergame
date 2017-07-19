a=zeros(250,500);
b=255*ones(250,500);
c=[a;b];
d=conv2(c,[0 ;1; 2; 3; 2; 1; 0]);
imshow(d);

