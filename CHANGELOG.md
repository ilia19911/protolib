# [1.0.0-dev.8](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.0.0-dev.7...v1.0.0-dev.8) (2025-08-20)


### Bug Fixes

* **assert:** ошибка assert в компиляции таргета ([284fb41](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/284fb41b58108211d62e30fd22fb1cea674a33e4))
* **assert:** ошибка assert в компиляции таргета ([85183d9](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/85183d985deb5555e5365addde06c706f55ed187))
* **conan:** была сломана сборка под arm ([d843b6a](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/d843b6a958e66bc276777519105341c727d62de8))
* **conan:** не брал версию из пакета ([b57c229](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/b57c2297d6fa67310806c134031015602274ea14))
* **conan:** небыло профайла default ([8dbffd6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/8dbffd64be4c29516970add6fabedcb8a8ea6dd4))
* **conan:** проблема со сборкой пакета из за gtest ([67554c6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/67554c635b0bc5a18e93a3352e0996483ee59aec))
* **container:** перенес ресет пакета в sendPacket . ([59563ea](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/59563eaa36649ac741f0ad7db0f46c27fa83581b))
* **container:** починил тесты ([cfec07e](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/cfec07ed4afae1c40f12074be60a0f559f511642))
* **devops:** перенос ci в отдельный проект ([81ed0b2](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/81ed0b2be328aef2fd1ac9a76dd36c23be1ef6b6))
* **lacte:** убрал lacte библиотеку ([060c08f](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/060c08fded3aa5558ee2a9625c66953f37ba38ab))
* **lacte:** убрал протокол lacte ([8a220ac](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/8a220aca02944cb4b643c11606c1519813d1dfc3))
* **root permission:** убрал установку пакетов в таске publish ([ca5ae85](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/ca5ae858ccda107ebcea316cd227a081092aa0e8))
* **rxContainer:** контейнер печатает дебажный вывод до отправки ([b940f4e](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/b940f4ea57e478623bf13a825dec3dede2360edc))
* **uart:** сделал полный raw uart ([ec1c30e](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/ec1c30e7cd93be5ecb38ea934ff6d162525fbab0))


### Features

* **cicd:** поменял ветку devops проекта ([9c8212a](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/9c8212ae3736a0634586e6f651d9b0698bbeebe9))
* **debug:** добавил вывод принятых полей если пакет недопринят ([4e9636d](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/4e9636d01ca9c2e985cd10d37483e1ec2d4a9c0f))
* **debug:** добавил дебажный вывод в полях ([e3437c6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/e3437c61257c9b18f63cac615daed055bb0a805a))
* **debug:** добавил дебажный вывод на поля ([45d4ef4](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/45d4ef45d370e8f5dcdf3dc039d0d093f33cf227))
* **debug:** поправил правила релиза ([9d2e11e](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/9d2e11e7eeb213e672e79a6386818bedc54a91fc))
* **debug:** сделал дебажный вывод ошибок приема ([8bd39f9](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/8bd39f9bdefd516ee02ec604af5579a5ef6bee40))
* **debug:** сделал простой вывод дебажной инфы при ошибках приема ([7ecf66a](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/7ecf66a1cbf963b65aee97e9ab39d6d3e7327f62))
* **debug:** сделал простой вывод дебажной инфы при ошибках приема ([20accab](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/20accab22db4c2a80e24c1459ccc42310f3a9171))
* **debug:** сделал тестовый вывод ([a6a63fe](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/a6a63feacba087aaaebd3f72d37d553d4c0b05ea))
* **devops:** перенес основную логику cicd в отдельный проект ([c18aeb5](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/c18aeb5a8e3a78fb51cf9d5f286de725009a95fb))
* **devops:** поправил путь ([0be4530](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/0be4530271bd5d750b9c70cad953294259fd45d4))
* **runner:** изменился тег ([c1dda60](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/c1dda60845207d4106f959cb55a846db864266f6))
* **tag creating:** выпуск коммита ([25588e6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/25588e667477c4a8946bfa1840cf217ab3d741ef))

# [1.0.0-dev.7](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.0.0-dev.6...v1.0.0-dev.7) (2025-08-20)


### Bug Fixes

* **assert:** убрал assert так как на таргете не компилируется ([1af9409](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/1af9409b7528f146fdc856b2646da5f2f23914ae))

# [1.0.0-dev.6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.0.0-dev.5...v1.0.0-dev.6) (2025-08-20)


### Bug Fixes

* **assert:** убрал assert так как под таргет не компилируется ([4bc425a](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/4bc425a1e6b394742f631ae5203be86df507ed25))

# [1.0.0-dev.5](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.0.0-dev.4...v1.0.0-dev.5) (2025-08-20)


### Bug Fixes

* **uart:** сделал uart raw ([dcb202b](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/dcb202b48a7ad0821da8a23af03e1254dc022fdb))

# [1.5.0](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.4.0...v1.5.0) (2025-08-16)


### Features

* **debug:** сделал тестовый вывод ([a6a63fe](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/a6a63feacba087aaaebd3f72d37d553d4c0b05ea))

# [1.4.0](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.3.0...v1.4.0) (2025-08-16)


### Features

* **debug:** сделал дебажный вывод ошибок приема ([8bd39f9](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/8bd39f9bdefd516ee02ec604af5579a5ef6bee40))

# [1.3.0](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.7...v1.3.0) (2025-08-16)


### Features

* **debug:** добавил дебажный вывод на поля ([45d4ef4](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/45d4ef45d370e8f5dcdf3dc039d0d093f33cf227))

## [1.2.7](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.6...v1.2.7) (2025-08-16)


### Bug Fixes

* **lacte:** убрал lacte библиотеку ([060c08f](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/060c08fded3aa5558ee2a9625c66953f37ba38ab))

## [1.2.6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.5...v1.2.6) (2025-08-16)


### Bug Fixes

* **lacte:** убрал протокол lacte ([8a220ac](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/8a220aca02944cb4b643c11606c1519813d1dfc3))

## [1.2.5](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.4...v1.2.5) (2025-08-15)


### Bug Fixes

* **conan:** проблема со сборкой пакета из за gtest ([67554c6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/67554c635b0bc5a18e93a3352e0996483ee59aec))

## [1.2.4](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.3...v1.2.4) (2025-08-15)


### Bug Fixes

* **conan:** была сломана сборка под arm ([d843b6a](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/d843b6a958e66bc276777519105341c727d62de8))

## [1.2.3](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.2...v1.2.3) (2025-08-15)


### Bug Fixes

* **conan:** не брал версию из пакета ([b57c229](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/b57c2297d6fa67310806c134031015602274ea14))

## [1.2.2](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.1...v1.2.2) (2025-08-15)


### Bug Fixes

* **conan:** небыло профайла default ([8dbffd6](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/8dbffd64be4c29516970add6fabedcb8a8ea6dd4))

## [1.2.1](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.2.0...v1.2.1) (2025-08-15)


### Bug Fixes

* **devops:** перенос ci в отдельный проект ([81ed0b2](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/81ed0b2be328aef2fd1ac9a76dd36c23be1ef6b6))

# [1.2.0](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.1.2...v1.2.0) (2025-08-14)


### Features

* **devops:** перенес основную логику cicd в отдельный проект ([c18aeb5](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/c18aeb5a8e3a78fb51cf9d5f286de725009a95fb))
* **devops:** поправил путь ([0be4530](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/0be4530271bd5d750b9c70cad953294259fd45d4))

## [1.1.2](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.1.1...v1.1.2) (2025-08-14)


### Bug Fixes

* **root permission:** убрал установку пакетов в таске publish ([ca5ae85](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/ca5ae858ccda107ebcea316cd227a081092aa0e8))

## [1.1.1](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.1.0...v1.1.1) (2025-08-14)


### Bug Fixes

* **rxContainer:** контейнер печатает дебажный вывод до отправки ([b940f4e](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/b940f4ea57e478623bf13a825dec3dede2360edc))

# [1.1.0](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/compare/v1.0.0...v1.1.0) (2025-08-12)


### Features

* **runner:** изменился тег ([c1dda60](https://gitlab.insitechdev.ru/comfort/embedded/libraries/protolib/commit/c1dda60845207d4106f959cb55a846db864266f6))

# 1.0.0 (2025-08-12)


### Features

* **tag creating:** выпуск коммита ([25588e6](https://gitlab.insitechdev.ru/comfort/embeded/libraries/protolib/commit/25588e667477c4a8946bfa1840cf217ab3d741ef))
