m = 0.11; 
Kx = 11;
Ky = 12;
Kz = 30;
dx = 0.7;
dy = 0.7;
dz = 0.8;

%Paramètres calculés pour atteindre l'amortissement et la fréquence naturelle souhaités
dxcmd = 480*m;
dycmd = 480*m;
Kxcmd = 90000*m;
Kycmd = 90000*m;

%Paramètres du système réel
%A = [0 1 0 0 0 0; -110 -7 0 0 0 0; 0 0 0 1 0 0; 0 0 -120 -7 0 0; 0 0 0 0 0 1; 0 0 0 0 -300 -8];
A = [0 1 0 0 0 0; -Kx/m -dx/m 0 0 0 0; 0 0 0 1 0 0; 0 0 -Ky/m -dy/m 0 0; 0 0 0 0 0 1; 0 0 0 0 -Kz/m -dz/m];
B = [0 0 0; 1/m 0 0; 0 0 0; 0 1/m 0; 0 0 0; 0 0 1/m];
C = [1 0 0 0 0 0; 0 0 1 0 0 0; 0 0 0 0 1 0];
D = [0 0 0; 0 0 0; 0 0 0];

%Fonction transfert de l'observateur
s = tf('s');
H = C*(inv(s*eye(6)-A))*B+D; 
p = 10*pole(H);
L = (place(A.', C.', p)).';

AmX = sqrt((dx^2)/(4*Kx*m));
AmY = sqrt((dy^2)/(4*Ky*m));
AmZ = sqrt((dz^2)/(4*Kz*m));

%Fonction transfert pour trouver les pôles souhaités pour la commande
Acmd = [0 1 0 0 0 0; -Kxcmd/m -dxcmd/m 0 0 0 0; 0 0 0 1 0 0; 0 0 -Kycmd/m -dycmd/m 0 0; 0 0 0 0 0 1; 0 0 0 0 -Kz/m -dz/m];
Hcmd = C*(inv(s*eye(6)-Acmd))*B+D;
pcmd = pole(Hcmd);
%Séparer les pôles sur chaque axe
pcmdX = pcmd(1:2,1);
pcmdY = pcmd(3:4,1);
pcmdZ = 0.1*pcmd(5:6,1);

%Trouver le gain K nécessaire
KxC = place(A(1:2,1:2),B(1:2,1),pcmdX);
KyC = place(A(3:4,3:4),B(3:4,2),pcmdY);
KzC = place(A(5:6,5:6),B(5:6,3),pcmdZ);


plot3(out.simout.Data(:,1),out.simout.Data(:,2),out.simout.Data(:,3));