% Script to Symbolically Solve for the Parameters of the Compound Cubic
% used for Fully Constrained Continuous Acceleration Profiles.
function solveCompoundCubic()
    % Define System and Parameters:
    sf = sym('sf');
    tf = sym('tf');
    syms va v0 vf a b c d e f g h ti ve
    syms v1 v2 t act
    v1 = a*t^3 + b*t^2 + c*t + d; % Velocity Profile during First Cubic Section
    v2 = e*t^3 + f*t^2 + g*t + h; % Velocity Profile during Second Cubic Section
    act = int(v1^2, t, [0, ti]) + int(v2, t, [ti, tf]); % Specific Action (integral of specific kinetic energy
    
    % Setup Constraint Equations and Solve System:
    vars = [ti, a,b,c,d, e,f,g,h];
    ticonstraints = [... % Special Constrains on Intersection Point of the Two Cubics, ti
        ti == tf/2, ... % Dumb Basic Constraint
        diff(act,ti) == 0, ... % Principle of Least Action
    ];
    for ticon = ticonstraints
        eqns = [...
            ticon, ...
            subs(v1,t,0) == v0, ...d == v0, ...
            subs(v1,t,ti) == subs(v2,t,ti),...a*ti^3 + b*ti^2 + c*ti + d == e*ti^3 + f*ti^2 + g*ti + h, ...
            subs(v2,t,tf) == vf,...e*tf^3 + f*tf^2 + g*tf + h == vf, ...
            subs(diff(v1,t),0) == 0,...c == 0, ...
            subs(diff(v1,t),ti) == 0,...3*a*ti^2 + 2*b*ti + c == 0, ...
            subs(diff(v2,t),ti) == 0,...3*e*ti^2 + 2*f*ti + g == 0, ...
            subs(diff(v2,t),tf) == 0,...3*e*tf^2 + 2*f*tf + g == 0, ...
            int(v1,t,[0,ti]) + int(v2,t,[ti,tf])==sf...(a/4)*ti^4 + (b/3)*ti^3 + (c/2)*ti^2 + d*ti + (e/4)*tf^4 + (f/3)*tf^3 + (g/2)*tf^2 + h*tf - (e/4)*ti^4 - (f/3)*ti^3 - (g/2)*ti^2 - h*ti == sf ...
        ];
        sols = solve(eqns,vars);
        dispResults(sols,vars);
    end
end
function dispResults(sols,vars)
    % Display Results:
    results = {};
    for i = 1:numel(vars)
        sol = sols.(char(vars(i)));
        simp = simplify(sol);
        for j = 1:numel(sol)
            fprintf( "%s = %s \n\t = %s\n", vars(i), sol(j), simp(j) );
            results{i,j} = sprintf( "%s = $$%s$$\n\n", vars(i), latex(simp(j)) );
        end
    end
    figure();
    hold on
        w = 0;
        hh = 0;
        for i = 1:size(results,1)
            tb = text(0,hh, results(i,:), 'Interpreter', 'latex');
            w = max(w, tb.Extent(3));
            hh = hh + tb.Extent(4);
        end
    hold off
    title('Pan Around to See All Equations', 'Interpreter', 'latex');
    % TODO (nice to have): fit figure to equations.
end