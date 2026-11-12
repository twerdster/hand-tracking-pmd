%% 

% curCMReal = [-0.030538, -0.062486, 0.455108];
% prevCMReal = curCMReal;
prevCMReal = 0;
curCMReal = 0;
curSupport = [int32(0), int32(0)];

for i = 100 : 182
[nextSupport, nextCM, nextCMReal] = Iterate(curSupport, W{20+i}, 120, 165, curCMReal, prevCMReal);
    curSupport = nextSupport;
    prevCMReal = curCMReal;
    curCMReal = nextCMReal;
    imagesc(W{20 + i}, [0.2 0.5]); hold on; colormap gray
    %imagesc(temp);
%     if(nextCM(3) == 1)
%         plot(nextCM(2)+1, nextCM(1)+1, '.','MarkerSize',60,'Color',[0 1 0]);
%          plot(nextCM(2)+1, nextCM(1)+1, '.','MarkerSize',40,'Color',[0 0 1]);
%     end
    hold off; drawnow;
    %writeVideo(part1, getframe);
%     if(nextCM(3) == 1)
%               temp(nextCM(1)+3,nextCM(2)+2) = 2;
%               temp(nextCM(1)+2,nextCM(2)+2) = 2;
%               temp(nextCM(1)+1,nextCM(2)+2) = 2;
%               temp(nextCM(1)+2,nextCM(2)+3) = 2;
%               temp(nextCM(1)+2,nextCM(2)+1) = 2;
% 
%     end
%     imagesc(temp, [0.3, 0.6]); drawnow;
end

% J2 = imresize(I, 2, 'nearest');