
ts=0.000001; % time step de simulation pour simulink
Fe=1/ts; % fréquence d'échantillonnage de la fft
t_sim=2; % 
sim('sh'); %lance la simulation du modèle Simulink
spectrum_monolat_1Hz(Fe, simout);
%hold on 


