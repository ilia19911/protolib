export default {
  branches: [
    { name: 'main' },                                   // стабильные релизы
    { name: 'develop', channel: 'develop', prerelease: 'dev' }, // пререлизы (например 1.2.3-rc.1)
    { name: 'testing', channel: 'testing', prerelease: 'rc' }  // ещё одна prerelease-ветка
  ],
  repositoryUrl: process.env.CI_PROJECT_URL || undefined,
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
    }]
  ]
}