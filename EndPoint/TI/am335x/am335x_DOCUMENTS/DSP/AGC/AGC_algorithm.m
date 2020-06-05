%AGC algorithm

%samples chunk fit to 4ms
N = 256;
%number of samples vector
x =0: 1: N-1;

%create input vector
sample_rate = 16000;
sn_ratio    = 100; %signal/noise
signal_freq = randi([300,3400],5, 1);
signal_amp  = 32768/size(signal_freq,1);
in_signal   = signal_amp * sin( 2 * pi * (signal_freq/sample_rate) * x);
in_signal   = ceil(in_signal); %round up to integeres
in_signal   = sum(in_signal);

%add noise
noise       = (randi([100,1000],256, 1))';
in_signal   = in_signal + noise;



% normalize signal
MAX_RANGE = 32767; % was observed from CDC work
in_sign_n = in_signal/MAX_RANGE;

plot(x, in_sign_n);
legend({'input signal normalized to 25000'}, ...
         'Location','northwest',             ...
         'Orientation','horizontal');
%             N-1     
%            ----     
%         1  \        2
% P    = --- /    x[n]        power of the signal
%         N  ---- 
%            n = 0

%             N-1     
%            ----     
%            \        2
% E    =     /    x[n]        energy of the signal
%            ---- 
%            n = 0
E=0; P=0;
for idx = 1:N
    val = in_sign_n(idx)^2;
    E   = E + val;       %energy
    P   = P + ( val/N ); %power
end
%  p (in db) = 10*log10(p/p_ref)
%  since the value of p_ref is taken as 1 Watt , equation becomes
%  p (in db) = 10*log10(p)
p_db = 10 * log10(P);
fprintf("Power of signal  : %s\n", num2str(p_db));
fprintf("Energy of signal : %s\n", num2str(E));

CDC_MAX = 25000; %observed
in_sign_max(1:N) = CDC_MAX/MAX_RANGE;
P_max = sum(in_sign_max.^2/N);
E_max = sum(in_sign_max.^2);
p_db_max = 10 * log10(P_max);

CDC_MIN = 1000; %proposed
in_sign_min(1:N) = CDC_MIN/MAX_RANGE;
P_min = sum(in_sign_min.^2/N);
E_min = sum(in_sign_min.^2);
p_db_min = 10 * log10(P_min);

p_db_norm = (p_db_max + p_db_min)/2;
P_norm = 10^(p_db_norm/10);
E_norm = P_norm * N;

%          output power     =     input power       * K_Coeff
% 
%              N-1                   N-1
%             ----                   ----
%          1  \        2          1  \        2
%         --- /    y[n]     =    --- /    x[n]      * K_Coeff
%          N  ----                N  ----
%             n = 0                  n = 0

%              N-1                   N-1
%             ----                   ----
%             \        2            \          2
%             /    y[n]     =       /    ( x[n]     * K_Coeff )
%             ----                  ----
%             n = 0                  n = 0 
% calculate the multiplication factor K for the signal
%       /-----------------------
% K = \/output_power/input_power

K = sqrt(P_norm / P);
out_signal = in_signal * K;

spl_x  = 0: 0.25: N-1;
in_spl_y  = spline(x,  in_signal,  spl_x);
out_spl_y = spline(x,  out_signal, spl_x);
plot(x,        in_signal,  'o', ...
     spl_x,    in_spl_y,        ...     
     x,        out_signal, 'x', ...
     spl_x,    out_spl_y,       ...
     x,        noise);
 
 legend({'input signal',                ...
         'input curved representation', ...
         'output signal',               ...
         'output curved representation',...
         'noise'},                      ...
         'Location','northwest',        ...
         'Orientation','horizontal');
grid on;
grid minor;