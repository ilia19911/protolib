export default {
  branches: [
    'main',
    { name: 'next', channel: 'next', prerelease: true },
    { name: 'beta', channel: 'beta', prerelease: true },
    { name: 'maintenance/*', range: 'patch' }
  ],
  // Единый формат тегов для релиза
  tagFormat: 'v${version}',
  // Для GitLab CI возьмем URL из переменных окружения
  repositoryUrl:
    process.env.CI_PROJECT_URL ||
    (process.env.CI_SERVER_URL && process.env.CI_PROJECT_PATH
      ? `${process.env.CI_SERVER_URL}/${process.env.CI_PROJECT_PATH}.git`
      : undefined),
  ci: true,
  plugins: [
    '@semantic-release/commit-analyzer',
    '@semantic-release/release-notes-generator',
    ['@semantic-release/changelog', { changelogFile: 'CHANGELOG.md' }],
    ['@semantic-release/git', {
      assets: ['CHANGELOG.md'],
      message: 'chore(release): ${nextRelease.version} [skip ci]\n\n${nextRelease.notes}'
    }],
    ['@semantic-release/gitlab', {
      gitlabUrl: process.env.CI_SERVER_URL,
      gitlabApiPathPrefix: '/api/v4'
    }],
    ['@semantic-release/exec', {
      // Выполняется только если релиз действительно произведен
      successCmd:
        'printf "SEMREL_RELEASED=true\\nSEMREL_VERSION=${nextRelease.version}\\nSEMREL_TAG=v${nextRelease.version}\\n" > semrel.env && echo ${nextRelease.version} > .pkg_version'
    }]
  ]
};