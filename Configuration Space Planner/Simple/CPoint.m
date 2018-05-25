% N-Dimensional Point Capable of Printing itself to a C-Array
classdef CPoint
    properties (GetAccess = public, SetAccess = private)
        pos;
        name;
    end % properties <- CPoint
    methods
        % Construct a Point from a Coordinate Vector.
        % Optionally, include a name for printing.
        function obj = CPoint(coords, name)
            obj.pos = coords(1,:); % Extract First Dmimension from Errant Two-Dimensional Entries
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
            str = sprintf("float CP_%s[%d] = %s;", id, numel(obj.pos), obj.toC_init(0));
            if(print)
                disp(str);
            end
        end % #toC
        
        % Creates a String of the Initializer Required to Create this Array
        % in C.
        % Optionally prints the initializer to the console.
        function str = toC_init(obj, print)
            str = "{" + sprintf("%0.5f, ", obj.pos(1:end-1)) + sprintf("%0.5f}", obj.pos(end));
            if(print)   
                disp(str);
            end
        end % #toC_init
            
    end % methods <- CPoint
end %CPoint