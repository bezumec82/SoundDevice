
in_size     =32;
out_size    =28;

%%32 samples of signal
t_in =0: 1: in_size-1;
t_out=0: 1: out_size-1;

signal_amp  =20000;
sample_rate =8000;
signal_freq =[555; 600; 523; 800];
in_signal   =signal_amp * sin( 2 * pi * (signal_freq/sample_rate) * t_in);
in_signal   =ceil(in_signal); %round up to integeres
in_signal   =sum(in_signal);

%%Show input signal
in_spl_x  = 0: 0.1:  in_size-1;
in_spl_y  = spline(t_in,  in_signal,  in_spl_x);
plot(t_in,      in_signal,  'o', ...
     in_spl_x,  in_spl_y);
legend({'input signal', 'input curved representation'}, ...
       'Location','northwest','Orientation','horizontal');
    grid on;
    grid minor;
    
%%generate sinc table
sinc_arg=zeros(out_size,in_size,'double');
sinc_val=zeros(out_size,in_size,'double');
for out_idx=1:out_size
    for in_idx=1:in_size
           sinc_arg(out_idx,in_idx) = pi*(out_idx*in_size/out_size-in_idx);
       if (sinc_arg(out_idx,in_idx)==0)
           sinc_val(out_idx,in_idx)=2^15;
       else
           sinc_val(out_idx,in_idx)=ceil(2^15 * sin(sinc_arg(out_idx,in_idx))/(sinc_arg(out_idx,in_idx)));
       end       
    end
end

%%apply sinc
out_signal=zeros(out_size,1,'double');
for out_idx=1:out_size
    for in_idx=1:in_size    
        out_signal(out_idx)= out_signal(out_idx) + ceil((in_signal(in_idx) * sinc_val(out_idx, in_idx))/2^15);
        fprintf("%-6d ", out_signal(out_idx));
    end
    fprintf("%d -> %d\n", out_idx, out_signal(out_idx));
end

%%demonstrate
out_spl_x = 0: 0.1: out_size-1;
out_spl_y = spline(t_out, out_signal, out_spl_x);
plot(t_in,      in_signal,  'o', ...
     in_spl_x,  in_spl_y,        ...
     t_out,     out_signal, 'x', ...
     out_spl_x, out_spl_y );
grid on;
grid minor;
lgd = legend({'input signal', 'input curved representation', ...
       'output signal', 'output curved representation'},     ...
       'Location','northwest','Orientation','horizontal');
