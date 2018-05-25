% Generates a Roadmap through the Given Binary Configuration Space Matrix
% using the Voronoi Tesselation which optimizes distance between the 
% Roadmap and Obstacles at all Points.
function voronoi_roadmap(cfg)
    config = cfg;
    % Add Thin Boundary Around Configuration Space (prevent overflow):
    [h_cfg,w_cfg] = size(config);
    config(:,1) = 1;
    config(1,:) = 1;
    config(:,w_cfg) = 1;
    config(h_cfg,:) = 1;
    
    B = bwboundaries(config); % Get Points along Boundary of Accessible and Inaccessible Space
    Bmat = cell2mat(B);
    
    [vs,cs] = voronoin(Bmat);
    
    function reg = region(x,y,r) % Returns the Region in the Configuration Space around the given Coords of half-width r
        xm = x-r; if(xm<1); xm = 1; end
        ym = y-r; if(ym<1); ym = 1; end
        xM = x+r; if(xM>w_cfg); xM = w_cfg; end
        yM = y+r; if(yM>h_cfg); yM = h_cfg; end
        reg = config(ym:yM, xm:xM);
    end
    
    % Collect Voronoi Edges which Don't Intersect an Obstacle -> Roadmap
    % Edges and Create a Connectivity Matrix for Use in A* Later.
    MC = zeros(numel(vs)); % Initialize Connectivity Matrix (starts at max-size, will be trimmed to len(vrm)xlen(vrm));
    vrm = []; % Initialize Vector of Roadmap Vertices    
    for i = (1:numel(cs)) % Loop Through each Voronoi Cell
        c = cs{i};
        for j = (1:numel(c)) % Check Each Pair of Vertices
            n = c(j);
            if(j==numel(c)) % Check if Loop-Around is Necessary
                m = c(1);
            else
                m = c(j+1);
            end
            xa = vs(n,2); ya = vs(n,1);
            xb = vs(m,2); yb = vs(m,1);
            if(xa~=Inf && ya~=Inf && xb~=Inf && yb~=Inf) % Catch special cases
                xxa = floor(xa); if(xxa == 0); xxa = 1; end % Get Nearest Image Coord
                yya = floor(ya); if(yya == 0); yya = 1; end
                xxb = floor(xb); if(xxb == 0); xxb = 1; end
                yyb = floor(yb); if(yyb == 0); yyb = 1; end
                
                acc_A = sum(sum(region(xxa,yya, 3))) == 0; % Whether the 6px Wide Region around Vertex A is Accessible
                acc_B = sum(sum(region(xxb,yyb, 3))) == 0; % Whether the 6px Wide Region around Vertex A is Accessible
                
                if(acc_A && acc_B) % If neither vertex is in an obstacle, add to roadmap (<-might present issues at edges of obstacle?... shouldn't be there)
                    % Check to see if vertex already is in roadmap and, thus,
                    % has index in connectivity matrix:
                    if(~isempty(vrm)) % Ensure List of Roadmap Vertices is Not Empty
                        ia = find(vrm(:,1)==xa & vrm(:,2)==ya);
                        if(isempty(ia)) % Vertex not yet in roadmap. Add it and assign index.
                            vrm = [vrm; xa,ya];
                            [ia, ~] = size(vrm);
                        end
                        ib = find(vrm(:,1)==xb & vrm(:,2)==yb);
                        if(isempty(ib)) % Vertex not yet in roadmap. Add it and assign index.
                            vrm = [vrm; xb,yb];
                            [ib, ~] = size(vrm);
                        end
                    else
                        [Lv, ~] = size(vrm);
                        vrm = [vrm; xa,ya; xb,yb];
                        ia = Lv+1; ib = Lv+2;
                    end
                    
                    MC(ia,ib) = 1; MC(ib,ia) = 1; % Add entry to Connectity Matrix. (can speed up this triangular matrix with sparse matrix?)
                end % in obstacle?
            end % is Inf?
        end % loop vertices
    end % loop cells
    % Trim Connectivity Matrix:
    [Lrm, ~] = size(vrm);
    MC = MC(1:Lrm, 1:Lrm);
    size(vrm)
    
    figure();
    imagesc(cfg);
    hold on
    
    %Follow Roadmap Connectivity Matrix to Plot Image
    [as,bs] = find(MC);
    for i = (1:numel(as))
        xs = [vrm(as(i),1), vrm(bs(i),1)];
        ys = [vrm(as(i),2), vrm(bs(i),2)];
        plot(xs,ys);
    end
    
    title('Joint-Angle Space Voronoi Roadmap');

    
    function idx=th2idx(th); idx=round((th+pi) ./ ((360/360) * (pi/180) / 2))+1; end % Convert Angle to Index in Config. Matrix
    th_ticks = (-360:15:360);
    ts = th2idx(th_ticks*pi/180);
    xticks(ts); yticks(ts);
    xticklabels(num2cell(th_ticks)); yticklabels(num2cell(th_ticks));
    xlabel('$$\theta_{2}=\theta_{B}  [deg]$$', 'Interpreter', 'latex');
    ylabel('$$\theta_{1}=\theta_{A}  [deg]$$', 'Interpreter', 'latex')
    grid on
    
    %plot(vrm(:,1), vrm(:,2));
    %plot(Bmat(:,2), Bmat(:,1));
    
end % #voronoi_roadmap