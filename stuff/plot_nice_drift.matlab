%x=load('/tmp/s1');

%plot(x)
%plot(diff(x))
%plot(filter(ones(1,7)/7,1,diff(x)))

%x1=diff(x);
%x2=x1-mean(x1);
%plot(x2)
%plot(filter(ones(1,7)/7,1,x2))

%x=ones(1,20)*2
%x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
%x = [2, 1, 2, 1, 2, 1, 200, 1, 2, 1, 2, 1, 2, 1, 2]

%plot(filter(ones(1, 4), 1, x))
%filter(ones(1, 4), 1, x)

%time_between_samples = 1  % each 1 second
%tau = 1
%tmp = time_between_samples/tau
%filter(tmp, [1 tmp-1], x)  % one-pole low-pass filter

% vim: set ft=matlab:
