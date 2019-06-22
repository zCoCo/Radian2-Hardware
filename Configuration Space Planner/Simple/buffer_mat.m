% Adds a Buffer of Size buff to the given workspace Matrix, m
% where 1 indicates an obstacle and 0 indicates accessible space.
function mat = buffer_mat(m, buff)
    [wrows,wcols] = size(m);
    %Create Circular Keepout Mask:
    [mxs, mys] = meshgrid(1:2*buff, 1:2*buff);
    mask = (mxs-buff-0.5).^2 + (mys-buff-0.5).^2;
    mask = (mask<=buff^2);
    %Pad Matrix:
    pad = ceil(buff + 1);
    padmat = zeros(wrows+2*pad, wcols+2*pad);
    padmat(pad+1:end-pad, pad+1:end-pad) = m;
    %Move Mask over Obstacles:
    for wrow = ((pad+1):(wrows+pad))
        for wcol = ((pad+1):(wcols+pad))
            if(m(wrow-pad,wcol-pad) == 1)
                minX = wcol - buff + 0.5;
                maxX = wcol + buff - 0.5;
                minY = wrow - buff + 0.5;
                maxY = wrow + buff - 0.5;

                padmat(minY:maxY, minX:maxX) = padmat(minY:maxY, minX:maxX) | mask;
            end
        end
    end
    %Recover Matrix:
    mat = padmat(pad+1:end-pad, pad+1:end-pad);
end % #buffer_mat