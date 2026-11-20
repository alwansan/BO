.class public final synthetic Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Ljava/lang/Runnable;


# instance fields
.field public final synthetic f$0:Lcom/racas/black/BundleActivity$1$1;

.field public final synthetic f$1:J

.field public final synthetic f$2:J

.field public final synthetic f$3:Ljava/lang/String;


# direct methods
.method public synthetic constructor <init>(Lcom/racas/black/BundleActivity$1$1;JJLjava/lang/String;)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-object p1, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$0:Lcom/racas/black/BundleActivity$1$1;

    iput-wide p2, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$1:J

    iput-wide p4, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$2:J

    iput-object p6, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$3:Ljava/lang/String;

    return-void
.end method


# virtual methods
.method public final run()V
    .locals 6

    .line 0
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$0:Lcom/racas/black/BundleActivity$1$1;

    iget-wide v1, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$1:J

    iget-wide v3, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$2:J

    iget-object v5, p0, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;->f$3:Ljava/lang/String;

    invoke-virtual/range {v0 .. v5}, Lcom/racas/black/BundleActivity$1$1;->lambda$update$0$com-racas-black-BundleActivity$1$1(JJLjava/lang/String;)V

    return-void
.end method
