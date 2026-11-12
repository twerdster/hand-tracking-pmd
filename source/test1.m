%%
for i=0:10
    try
        pmdClose(i);
    catch
        fprintf('No such handle:%i\n',i);
    end
end

hnd = pmdOpen ('camboardnano.W64.pap', '','camboardnanoproc.W64.ppp', '');
% Set first integration time to 1 ms
pmdSetIntegrationTime (hnd, 0, 1000)
%curSupport = int32(0);
curSupport = [int32(0), int32(0)];
% prevCMReal = 0;
% curCMReal = 0;
curCMReal = 0;
prevCMReal = 0;
nextCM = 0;
nextCMReal = 0;
%%
while 1
    pmdUpdate (hnd);
    F = pmdGetFlags (hnd);
    dist = pmdGetDistances (hnd);
    BW = F==0;
    %imagesc(flipud((dist.*BW)'),[0.1 0.4]);
    D = (dist.*BW)';%pmdGetAmplitudes(hnd);
      [nextSupport, nextCM, nextCMReal] = Iterate(curSupport, D, 120, 165, curCMReal, prevCMReal);
      curSupport = nextSupport;
      prevCMReal = curCMReal;
      curCMReal = nextCMReal;
      imagesc(D, [0.2 0.5]); hold on; colormap gray
      if(nextCM(3) == 1)
          plot(nextCM(2)+1, nextCM(1)+1, '.','MarkerSize',60,'Color',[0 1 0]);
          plot(nextCM(2)+1, nextCM(1)+1, '.','MarkerSize',40,'Color',[0 0 1]);
      end
      if(nextCM(6) == 1)
          plot(nextCM(5)+1, nextCM(4)+1, '.','MarkerSize',60,'Color',[0 1 0]);
          plot(nextCM(5)+1, nextCM(4)+1, '.','MarkerSize',40,'Color',[0 0 1]);
      end
      hold off; drawnow;
      %writeVideo(demo, getframe);
end
% 
% % % J2 = imresize(I, 2, 'nearest');
% % 
% 
% %%
% for i=0:6
%     try
%         pmdClose(i);
%     catch
%         fprintf('No such handle:%i\n',i);
%     end
% end
% 
% hnd = pmdOpen ('camboardnano.W64.pap', '','camboardnanoproc.W64.ppp', '')
% % Set first integration time to 1 ms
% pmdSetIntegrationTime (hnd, 0, 1000)
% 
% %%
% % curCMReal = [-0.030538, -0.062486, 0.455108];
% % prevCMReal = curCMReal;
% while 1
%     pmdUpdate (hnd);
%     F = pmdGetFlags (hnd);
%     dist = pmdGetDistances (hnd);
%     BW = F==0;
%     %imagesc(flipud((dist.*BW)'),[0.1 0.4]);
%     D = (dist.*BW)';%pmdGetAmplitudes(hnd);
%     
% %     [nextCM, nextCMReal] = Hand(D, 120, 165, curCMReal, prevCMReal);
% %     prevCMReal = curCMReal;
% %     curCMReal = nextCMReal;
%     temp = D;
% %     temp(nextCM(1)-1,nextCM(2)) = 2;
% %     temp(nextCM(1),nextCM(2)) = 2;
% %     temp(nextCM(1) + 1,nextCM(2)) = 2;
% %     temp(nextCM(1),nextCM(2)-1) = 2;
% %     temp(nextCM(1),nextCM(2)+1) = 2;
%     imagesc(temp); drawnow;
% end
% 
% % J2 = imresize(I, 2, 'nearest');