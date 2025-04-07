function spectrum_monolat_1Hz(Fe, x)

% Fe doit être un entier

n = floor(length(x)/Fe);

if n==0

	disp('erreur : le temps de simulation est inférieur à 1s')

else

	x = x(1:(n*Fe));        


	power = zeros(Fe,1);

		for k = 1:n

		xt = x( 1+(k-1)*Fe : k*Fe);

		y = ( abs(fft(xt))/Fe ).^2;

		power = power + y;
        
        k = k+1;

		end


	power = power/n;


if (floor(Fe/2) - Fe/2)      % Fe et donc le nombre de points de power sont impairs

  S = [power(1), power(2:(Fe+1)/2)'.*2];
  f = [0:1:Fe/2 - df/2 ];

else      % Fe et donc le nombre de points de power sont pairs

  S = [power(1), power(2:Fe/2+1)'.*2];
  f = [0:1:Fe/2];

assignin('base', 'Sf', S)

end

figure(1)

subplot(3,1,1) % 3 rows, 1 column, first position
loglog(f,S);
title('PSD loglog')
xlabel 'f (Hz)';
ylabel 'V^2';

fmax=2000;

subplot(3,1,2) % 3 rows, 1 column, second position
semilogy(f(1:fmax),S(1:fmax));
title('PSD semilog')
xlabel 'f (Hz)';
ylabel 'V^2';

fmax=200;

subplot(3,1,3) % 3 rows, 1 column, second position
plot(f(1:fmax),S(1:fmax));
title('PSD linear')
xlabel 'f (Hz)';
ylabel 'V^2';

end