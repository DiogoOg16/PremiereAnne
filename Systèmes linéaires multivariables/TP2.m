m = 0.11; 
Kx = 11;
Ky = 12;
Kz = 30;
dx = 0.7;
dy = 0.7;
dz = 0.8;

%Erreurs de l'observateur 
mobs = m*1.5; %0.1;
dxobs = dx*2;
dyobs = dy*2;
dzobs = dz*2;
Kxobs = Kx*3;
Kyobs = Ky*3;
Kzobs = Kz*3;

%A = [0 1 0 0 0 0; -110 -7 0 0 0 0; 0 0 0 1 0 0; 0 0 -120 -7 0 0; 0 0 0 0 0 1; 0 0 0 0 -300 -8];
A = [0 1 0 0 0 0; -Kx/m -dx/m 0 0 0 0; 0 0 0 1 0 0; 0 0 -Ky/m -dy/m 0 0; 0 0 0 0 0 1; 0 0 0 0 -Kz/m -dz/m];
B = [0 0 0; 1/m 0 0; 0 0 0; 0 1/m 0; 0 0 0; 0 0 1/m];
C = [1 0 0 0 0 0; 0 0 1 0 0 0; 0 0 0 0 1 0];
D = [0 0 0; 0 0 0; 0 0 0];

%Paramètres de l'observateur
Aob = [0 1 0 0 0 0; -Kxobs/mobs -dxobs/mobs 0 0 0 0; 0 0 0 1 0 0; 0 0 -Kyobs/mobs -dyobs/mobs 0 0; 0 0 0 0 0 1; 0 0 0 0 -Kzobs/mobs -dzobs/mobs];
Bob = [0 0 0; 1/mobs 0 0; 0 0 0; 0 1/mobs 0; 0 0 0; 0 0 1/mobs];

Ob = obsv(A,C);
Co = ctrb(A,B);

s = tf('s');
H = C*(inv(s*eye(6)-Aob))*Bob+D; 
p = 10*pole(H);
L = (place(Aob.', C.', p)).';

AmX = sqrt((dx^2)/(4*Kx*m));
AmY = sqrt((dy^2)/(4*Ky*m));
AmZ = sqrt((dz^2)/(4*Kz*m));

plot3(out.simout.Data(:,1),out.simout.Data(:,2),out.simout.Data(:,3));