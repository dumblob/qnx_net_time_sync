x=octave:1> x=load('/tmp/s1');
octave:2> plot(x)
octave:3> plot(diff(x))
octave:4> plot(filtdiff(x))
filt
filter
filter2
octave:4> plot(filter(ones(1,7)/7,1,diff(x)))
octave:5> x1=diff(x);
octave:6> x2=x1-mean(x1);
octave:7> plot(x2)
octave:8> plot(filter(ones(1,7)/7,1,x2))
