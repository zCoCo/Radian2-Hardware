function plot_data()
    [conn, no_conn, times] = results();
     % N.B.: coil currents have same amplitude and are 90deg out of phase .: norm of currents gives instantaneous peak current (wave mag.)
    rms_conn = sqrt(conn(:,1).^2 + conn(:,2).^2) * sqrt(2)/2;
    rms_no_conn = sqrt(no_conn(:,1).^2 + no_conn(:,2).^2) * sqrt(2)/2;
    
    figure();
    hold on
        plot(times, conn(1:length(times),1));
        plot(times, conn(1:length(times),2));
        plot(times, rms_conn(1:length(times)));
        plot(times, no_conn(1:length(times),1));
        plot(times, no_conn(1:length(times),2));
        plot(times, rms_no_conn(1:length(times)));
    hold off
    xlabel('Current [mA]', 'Interpreter','latex');
    ylabel('Time [ms]', 'Interpreter','latex');
    legend({...
        'Coil A Current, with Motor Connected',...
        'Coil B Current, with Motor Connected',...
        'RMS Current, with Motor Connected',...
        'Coil A Current, with Motor Disconnected',...
        'Coil B Current, with Motor Disconnected',...
        'RMS Current, with Motor Disconnected',...
    }, 'Interpreter','latex');
    
end