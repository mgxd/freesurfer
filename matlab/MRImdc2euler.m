function eulerangles = MRImdc2euler(Mdc)
% eulerangles = MRImdc2euler(Mdc)
%
% Solves for the Euler angles given the 3x3 matrix of direction cosines.
%
% $Id: MRImdc2euler.m,v 1.1 2005/10/12 05:40:27 greve Exp $

eulerangles = [];
if(nargin ~= 1)
  fprintf('eulerangles = MRImdc2euler(Mdc)\n');
  return;
end

theta = acos(Mdc(3,3));
phi = asin(Mdc(3,2)/(sin(theta)+eps));
psi = asin(Mdc(2,3)/(sin(theta)+eps));

eulerangles = [theta phi psi];

return;



