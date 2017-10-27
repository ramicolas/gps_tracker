function varargout = fileconv(varargin)
% FILECONV M-file for fileconv.fig
%      FILECONV, by itself, creates a new FILECONV or raises the existing
%      singleton*.
%
%      H = FILECONV returns the handle to a new FILECONV or the handle to
%      the existing singleton*.
%
%      FILECONV('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in FILECONV.M with the given input arguments.
%
%      FILECONV('Property','Value',...) creates a new FILECONV or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before fileconv_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to fileconv_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help fileconv

% Last Modified by GUIDE v2.5 20-Dec-2016 16:22:07

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @fileconv_OpeningFcn, ...
                   'gui_OutputFcn',  @fileconv_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before fileconv is made visible.
function fileconv_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to fileconv (see VARARGIN)

% Choose default command line output for fileconv
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes fileconv wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = fileconv_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[filename pathname] = uigetfile({'*.txt'},'File Selector');
fullpathname = strcat(pathname, filename);
M = csvread(fullpathname);
assignin('base','M',M); % IF YOU WANT TO SHOW IN WORKSPACE
set(handles.text3, 'String','DONE')

FileNameProposed = sprintf('20%d-%d-%d.GPX',M(1,4),M(1,5),M(1,6));
[FileNameout,PathNameout] = uiputfile(FileNameProposed,'Save file name');
fullpathnameout = strcat(PathNameout, FileNameout);

fileID = fopen(fullpathnameout,'w');
fprintf(fileID,'<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<gpx creator=\"strava.com Android\">\n <metadata>\n');
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
fprintf(fileID,' </metadata>\n');
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


% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
