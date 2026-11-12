%%
hnd = pmdOpen ('camboardnano.W64.pap', '','camboardnanoproc.W64.ppp', '')

% Set first integration time to 1 ms
pmdSetIntegrationTime (hnd, 0, 1000)

%%
W={};
% Camboard nano is optimized for between 5cm to 50cm
distThr = 0.5;

% This is a reasonable sequence length
numImages = 3000;
close all;
pause(2);

for p = 1:numImages;    
    
    flags = pmdGetFlags (hnd);
    dist = pmdGetDistances (hnd);
    
    BW = flags==0;
    dist(~BW) = nan;
    dist = fliplr(rot90(dist));
    dist(dist>distThr) = nan;
    
    W{p} = dist;
    
    imagesc(dist, [0 0.5]);
    drawnow
    pmdUpdate (hnd);
    fprintf('Captured image: %i / %i\n', p, numImages);
end

save EasyMotions W