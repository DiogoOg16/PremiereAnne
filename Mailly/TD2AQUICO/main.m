
ts=0.000001; % time step de simulation pour simulink
Fe=1/ts; % fr�quence d'�chantillonnage de la fft
t_sim=2; % 
sim('bruit_TD2'); %lance la simulation du mod�le Simulink
spectrum_monolat_1Hz(Fe, simout2);
%hold on 


Vrms = (norm(simout2))/sqrt(Fe*t_sim)
