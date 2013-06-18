%
% My translation of novasc31 precession() routine to matlab
%

function [ra2,dec2] = precession (tdb1, tdb2,  ra, dec )

    persistent is_initialized = 0;
    persistent t_last = 0.0;
    persistent xx;
    persistent yx;
    persistent zx;
    persistent xy;
    persistent yy;
    persistent zy;
    persistent xz;
    persistent yz;
    persistent zz;

    % TDB Julian date of epoch J2000.0.
    T0 = 2451545.00000000;

    % Angle conversion constant
    ASEC2RAD = 4.848136811095359935899141e-6;

    eps0 = 84381.406;


    
    % If necessary, compute Julian dates.
    
    if (tdb1 < 10000.0)
      tdb1 = T0 + (tdb1 - 2000.0) * 365.25;
    end

    if (tdb2 < 10000.0)
      tdb2 = T0 + (tdb2 - 2000.0) * 365.25;
    end



    % Check to be sure that either 'tdb1' or 'tdb2' is equal to T0.
    if ((tdb1 ~= T0) && (tdb2 ~= T0))
        error('one of tdb1 or tdb2 must be TDB Julian date of epoch J2000.0');
    end

    % 't' is time in TDB centuries between the two epochs.
    t = (tdb2 - tdb1) / 36525.0;
    if (tdb2 == T0)
      t = -t;
    end


    if ( (is_initialized == 0) || ( abs (t - t_last) >= 1.0e-15))

        % Numerical coefficients of psi_a, omega_a, and chi_a, along with
        % epsilon_0, the obliquity at J2000.0, are 4-angle formulation from
        % Capitaine et al. (2003), eqs. (4), (37), & (39).

        psia   = ((((-    0.0000000951  * t
                     +    0.000132851 ) * t
                     -    0.00114045  ) * t
                     -    1.0790069   ) * t
                     + 5038.481507    ) * t;

        omegaa = ((((+    0.0000003337  * t
                     -    0.000000467 ) * t
                     -    0.00772503  ) * t
                     +    0.0512623   ) * t
                     -    0.025754    ) * t + eps0;

        chia   = ((((-    0.0000000560  * t
                     +    0.000170663 ) * t
                     -    0.00121197  ) * t
                     -    2.3814292   ) * t
                     +   10.556403    ) * t;

        eps0 = eps0 * ASEC2RAD;
        psia = psia * ASEC2RAD;
        omegaa = omegaa * ASEC2RAD;
        chia = chia * ASEC2RAD;

        sa = sin (eps0);
        ca = cos (eps0);
        sb = sin (-psia);
        cb = cos (-psia);
        sc = sin (-omegaa);
        cc = cos (-omegaa);
        sd = sin (chia);
        cd = cos (chia);

        % Compute elements of precession rotation matrix equivalent to
        % R3(chi_a) R1(-omega_a) R3(-psi_a) R1(epsilon_0).

        xx =  cd * cb - sb * sd * cc;
        yx =  cd * sb * ca + sd * cc * cb * ca - sa * sd * sc;
        zx =  cd * sb * sa + sd * cc * cb * sa + ca * sd * sc;
        xy = -sd * cb - sb * cd * cc;
        yy = -sd * sb * ca + cd * cc * cb * ca - sa * cd * sc;
        zy = -sd * sb * sa + cd * cc * cb * sa + ca * cd * sc;
        xz =  sb * sc;
        yz = -sc * cb * ca - sa * cc;
        zz = -sc * cb * sa + cc * ca;

        t_last = t;
        is_initialized = 1;
    end


    % convert angular coordinates to vectors

    pos1 = [
        cos(ra)*cos(dec),
        sin(ra)*cos(dec),
        sin(dec)
        ];

    % Perform rotation

    if (tdb2 == T0)
        % Perform rotation from epoch to J2000.0.
        pos2 = [
            xx * pos1(1) + xy * pos1(2) + xz * pos1(3),
            yx * pos1(1) + yy * pos1(2) + yz * pos1(3),
            zx * pos1(1) + zy * pos1(2) + zz * pos1(3)
            ];
    else
        % Perform rotation from J2000.0 to epoch.
        pos2 = [
            xx * pos1(1) + yx * pos1(2) + zx * pos1(3),
            xy * pos1(1) + yy * pos1(2) + zy * pos1(3),
            xz * pos1(1) + yz * pos1(2) + zz * pos1(3)
            ];
    end


    
    % Convert vectors back to angular components for output.
    
    xyproj = sqrt (pos2(1) * pos2(1) + pos2(2) * pos2(2));
    dec2 = atan2 (pos2(3), xyproj);

    if (xyproj == 0.0)
        ra2 = 0.0;
    else
        if ( (ra2 = atan2 (pos2(2), pos2(1))) < 0 )
            ra2 += 2*pi;
        end
    end


end
