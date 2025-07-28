package ru.nikita.t1;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

import java.util.Set;

@Getter
@Setter
@Entity
@Table(name = "users")
public class User {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(unique = true, nullable = false)
    private String login;

    @Column(nullable = false)
    private byte[] passwordHash;

    @Column(unique = true, nullable = false)
    private String email;

    // роли хранятся в виде строки разделённой запятыми ("admin,premium_user,guest")
    @Column(nullable = false)
    private Set<String> roles;

    public User() {}
}
