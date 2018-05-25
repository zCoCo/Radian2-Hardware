% List of CPoints, N-Dimensional Points Capable of Printing into to a
% C-Array or an Array of Points.
classdef CPointList
    properties (GetAccess = public, SetAccess = public)
        list;
        name;
    end % properties <- CPointList
    methods
        % Accepts a List of CPoints
        % Optionally, include a name for printing.
        function obj = CPointList(coords, name)
            obj.list = coords(1,:); % Extract First Dmimension from Errant Two-Dimensional Entries
            if nargin > 1
                obj.name = name;
            else
                obj.name = "";
            end
        end %% Constructor
        
        % Creates a String to Create this Point as an Array in C.
        % Optionally printing it to the console.
        function str = toC(obj, print)
            if strcmp(obj.name, "")
                id = inputname(1);
            else
                id = obj.name;
            end
            
            str = sprintf("float CPL_%s[%d] = {", id, numel(obj.list));
            for i = (1:numel(obj.list)-1)
                str = str + obj.list(i).toC_init(0) + ", ";
            end
            str = str + obj.list(end).toC_init(0) + "};";
            if(print)
                disp(str);
            end
        end
    end % methods <- CPointList
end %CPointList