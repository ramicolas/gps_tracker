%%

fileID = fopen('test2.gpx','w');
fprintf(fileID,'<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<gpx creator=\"strava.com Android\">\n <metadata>\n');
fprintf(fileID,'<gpx creator=\"strava.com Android\">\n');
fprintf(fileID,' <metadata>\n');
fprintf(fileID,'  <time>20%d-',M(1,4));
if M(1,5)<10
    fprintf(fileID,'0');
end
fprintf(fileID,'%d-',M(1,5));
if M(1,6)<10
    fprintf(fileID,'0');
end
fprintf(fileID,'%dT',M(1,6));
if M(1,7)<10
    fprintf(fileID,'0');
end
fprintf(fileID,'%d:',M(1,7));
if M(1,8)<10
    fprintf(fileID,'0');
end
fprintf(fileID,'%d:',M(1,8));
if M(1,9)<10
    fprintf(fileID,'0');
end
fprintf(fileID,'%dZ</time>\n',M(1,9));
fprintf(fileID,' <metadata>\n');
fprintf(fileID,' <trk>\n');
fprintf(fileID,'  <name>Run</name>\n');
fprintf(fileID,'  <trkseg>\n');

for i =1:length(M(:,1))
    fprintf(fileID,'   <trkpt lat="%9.7f" lon="%9.7f">\n',M(i,1),M(i,2));
    fprintf(fileID,'    <ele>%5.1f</ele>\n',M(i,3));
    
    fprintf(fileID,'  <time>20%d-',M(i,4));
    if M(i,5)<10
        fprintf(fileID,'0');
    end
    fprintf(fileID,'%d-',M(i,5));
    if M(i,6)<10
        fprintf(fileID,'0');
    end
    fprintf(fileID,'%dT',M(i,6));
    if M(i,7)<10
        fprintf(fileID,'0');
    end
    fprintf(fileID,'%d:',M(i,7));
    if M(i,8)<10
        fprintf(fileID,'0');
    end
    fprintf(fileID,'%d:',M(i,8));
    if M(i,9)<10
        fprintf(fileID,'0');
    end
    fprintf(fileID,'%dZ</time>\n',M(i,9));
    fprintf(fileID,'   </trkpt>\n');
end
fprintf(fileID,'  </trkseg>\n');
fprintf(fileID,' </trk>\n');
fprintf(fileID,'</gpx>\n');


fclose(fileID);