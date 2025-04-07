
ts=0.000001; % time step de simulation pour le mod�le simulink "sigma_delta_1er_ordre"

t_sim=10; % temps de simulation pour Simulink ('-ts' pour avoir un nombre entier de p�riodes si les signaux ont une fr�quence enti�re)
fck=1000000; % fr�quence d'horloge du modulateur
noise_laplace=10^-10; %  
noise=1e4/fck^2;  % bruit adapt� pour le sigma delta du 1er ordre avec integrateur en z
noise_2=0  % bruit pour le sigma delta du 2nd ordre
fc=10; % fr�quence de coupure du filtre passe-bas
tau=1/(2*pi*fc);

% sim('sigma_delta_1er_ordre'); %lance la simulation du mod�le Simulink
% spectrum_monolat_1Hz(fck, simout);

sim('sigma_delta_1er_ordre_z'); %lance la simulation du mod�le Simulink
 spectrum_monolat_loglog_1Hz(fck, simout);
hold on
 B= 3%gain du feedback sur le second �tage (essayer des valeurs comprises entre 0.01 et 1000)
 sim('sigma_delta_2nd_ordre_z'); %lance la simulation du mod�le Simulink
 spectrum_monolat_loglog_1Hz(fck, simout);

 hold on %permet de conserver les courbes pr�c�dentes dans la figure 1






% sim('sigma_delta_1er_ordre_TP'); %lance la simulation du mod�le Simulink
% spectrum_monolat_1Hz(1/ts, simout);

% noise_laplace=1e-5/fck^2; % bruit adapt� pour l'integrateur en Laplace (1/s)


