package ru.nikita.t1;

import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.stereotype.Service;
import ru.nikita.t1.dto.*;

import java.util.Arrays;
import java.util.Date;
import java.util.Set;

@Service
@RequiredArgsConstructor
public class AuthenticationService {

    private static final long ACCESS_TOKEN_VALIDITY_SECONDS = 5*60; // 5 минут
    private static final long REFRESH_TOKEN_VALIDITY_SECONDS = 7*24*60*60; // 7 дней
    private static final String SIGNING_KEY = "secretkey";

    private final BCryptPasswordEncoder bCryptPasswordEncoder;
    private final UserRepository userRepository;
    private final JwtBlacklistService jwtBlacklistService;

    public ResponseEntity<?> register(RegistrationRequest request) {
        if (userRepository.existsByLogin(request.getLogin()) || userRepository.existsByEmail(request.getEmail())) {
            throw new BadCredentialsException("Пользователь с таким именем или почтой уже зарегистрирован");
        }

        var user = new User();
        user.setLogin(request.getLogin());
        user.setEmail(request.getEmail());
        user.setPasswordHash(bCryptPasswordEncoder.encode(request.getPassword()).getBytes());
        user.setRoles(Set.of("guest"));
        userRepository.save(user);

        return ResponseEntity.ok().body(new MessageResponse("Пользователь успешно зарегистрирован!"));
    }

    public ResponseEntity<?> authenticate(LoginRequest request) {
        var user = userRepository.findByLogin(request.getLogin());
        if (!bCryptPasswordEncoder.matches(request.getPassword(), Arrays.toString(user.getPasswordHash()))) {
            throw new BadCredentialsException("Неверный логин или пароль");
        }

        var accessToken = generateAccessToken(user);
        var refreshToken = generateRefreshToken(user);

        return ResponseEntity.ok().body(new AccessToken(accessToken, refreshToken));
    }

    public ResponseEntity<?> refreshToken(RefreshTokenRequest request) {
        Claims claims = validateAndExtractClaimsFromJwt(request.getRefreshToken());
        if(jwtBlacklistService.isRevoked(claims)) {
            throw new InvalidTokenException("Токен был отозван.");
        }

        var user = userRepository.findByLogin(claims.getSubject());
        var newAccessToken = generateAccessToken(user);
        return ResponseEntity.ok().body(new AccessToken(newAccessToken, request.getRefreshToken()));
    }

    public ResponseEntity<?> revokeToken(RevokeTokenRequest request) {
        Claims claims = validateAndExtractClaimsFromJwt(request.getToken());
        jwtBlacklistService.addToBlacklist(claims);
        return ResponseEntity.ok().body(new MessageResponse("Токен успешно отозван."));
    }

    private String generateAccessToken(User user) {
        return Jwts.builder()
                .setSubject(user.getLogin())
                .claim("roles", user.getRoles())
                .setIssuedAt(new Date(System.currentTimeMillis()))
                .setExpiration(new Date(System.currentTimeMillis() + ACCESS_TOKEN_VALIDITY_SECONDS * 1000))
                .signWith(SignatureAlgorithm.HS512, SIGNING_KEY)
                .compact();
    }

    private String generateRefreshToken(User user) {
        return Jwts.builder()
                .setSubject(user.getLogin())
                .claim("roles", user.getRoles())
                .setIssuedAt(new Date(System.currentTimeMillis()))
                .setExpiration(new Date(System.currentTimeMillis() + REFRESH_TOKEN_VALIDITY_SECONDS * 1000))
                .signWith(SignatureAlgorithm.HS512, SIGNING_KEY)
                .compact();
    }

    private Claims validateAndExtractClaimsFromJwt(String token) {
        return Jwts.parserBuilder()
                .setSigningKey(SIGNING_KEY)
                .build()
                .parseClaimsJws(token)
                .getBody();
    }
}